import socket
import serial
import time
import sys
import threading
import tkinter as tk
from tkinter import scrolledtext, messagebox
import queue
import collections # Used for a fixed-size data buffer (deque)

UDP_IP = "127.0.0.1"
UDP_PORT = 5005
SERIAL_PORT = '/dev/ttyUSB0' # Change this to your serial port (e.g., 'COM3' on Windows)
BAUD_RATE = 9600

# DataHandler class remains nearly identical to the previous Tkinter version
class DataHandler:
    """Class to manage data transfer functions."""
    def __init__(self, log_queue, data_queue):
        self.log_queue = log_queue
        self.data_queue = data_queue 
        self.ser = None
        self.udp_sock_send = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.stop_event = threading.Event()
        self.receiver_socket = None

    def log_message(self, message, source="Info"):
        self.log_queue.put(f"[{source}] {message}\n")

    def udp_receiver_thread(self):
        self.receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            self.receiver_socket.bind((UDP_IP, UDP_PORT))
            self.log_message(f"Listening for UDP on {UDP_IP}:{UDP_PORT}...", "System")
        except socket.error as e:
            self.log_message(f"Could not bind UDP receiver: {e}", "Error")
            self.stop_event.set()
            return
            
        while not self.stop_event.is_set():
            try:
                self.receiver_socket.settimeout(0.5) 
                data, addr = self.receiver_socket.recvfrom(1024)
                message_str = data.decode('utf-8').strip()
                
                # Only log raw receipt if not processing quickly
                
                float_values = [float(x.strip()) for x in message_str.split(',')]
                
                if len(float_values) == 4:
                    # We only send f1 for oscilloscope plot
                    self.data_queue.put(float_values[0]) 
                    # self.log_message(f"Parsed floats: {float_values}", "UDP Parse")
                else:
                    self.log_message(f"Error: Expected 4 floats, found {len(float_values)}", "Error")
            except socket.timeout:
                continue
            except ValueError as e:
                self.log_message(f"Error parsing UDP floats: {e}", "Error")
            except Exception as e:
                if not self.stop_event.is_set():
                    self.log_message(f"UDP Receiver Error: {e}", "Error")
                    break
        self.cleanup_sockets()
        self.log_message("UDP Receiver stopped.", "System")
        
    # Sender thread and stop/start/cleanup functions remain the same as previous script

    def udp_serial_sender_thread(self):
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.5)
            self.log_message(f"Serial port {SERIAL_PORT} opened.", "Serial Info")
            self.log_message(f"Sending UDP to {UDP_IP}:{UDP_PORT}", "UDP Send")
            
            while not self.stop_event.is_set():
                if self.ser.in_waiting > 0:
                    serial_line = self.ser.readline().decode('utf-8').strip()
                    self.log_message(f"Read serial: {serial_line}", "Serial Recv")

                    try:
                        float_values = [float(x.strip()) for x in serial_line.split(',')]

                        if len(float_values) == 4:
                            udp_message = ",".join(map(str, float_values))
                            self.udp_sock_send.sendto(bytes(udp_message, "utf-8"), (UDP_IP, UDP_PORT))
                            self.log_message(f"Sent UDP: {udp_message}", "UDP Send")
                        else:
                            self.log_message(f"Warning: Serial data did not contain 4 floats. Data: {serial_line}", "Warning")
                    except ValueError as e:
                        self.log_message(f"Error parsing serial data: {e}", "Error")

        except serial.SerialException as e:
            self.log_message(f"Error opening serial port: {e}", "Error")
            self.log_message("Please check the SERIAL_PORT variable and ensure pyserial is installed.", "Error")
        except Exception as e:
            if not self.stop_event.is_set():
                self.log_message(f"Sender Error: {e}", "Error")
        finally:
            if self.ser and self.ser.isOpen():
                self.ser.close()
                self.log_message("Serial port closed.", "Serial Info")
            self.cleanup_sockets()
            self.log_message("Sender background process finished.", "System")

    def cleanup_sockets(self):
        if self.udp_sock_send:
            try:
                self.udp_sock_send.close()
            except socket.error:
                pass
        if self.receiver_socket:
            try:
                self.receiver_socket.close()
            except socket.error:
                pass

    def start_process(self, process_type):
        if not self.stop_event.is_set():
            messagebox.showwarning("Warning", "A process is already running. Please stop it first.")
            return
        
        self.stop_event.clear()
        self.udp_sock_send = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        if process_type == 'sender':
            thread_func = self.udp_serial_sender_thread
        elif process_type == 'receiver':
            thread_func = self.udp_receiver_thread
        else:
            return

        data_thread = threading.Thread(target=thread_func, daemon=True)
        data_thread.start()

    def stop_process(self):
        self.log_message("Stop command issued...", "System")
        self.stop_event.set()
        self.cleanup_sockets()


class MainGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("UDP/Serial Interface GUI (Oscilloscope View)")
        self.geometry("600x600")
        
        self.log_queue = queue.Queue()
        self.data_queue = queue.Queue() 
        self.handler = DataHandler(self.log_queue, self.data_queue)
        
        self.canvas_height = 250
        self.canvas_width = 580
        self.padding = 30 
        self.data_history_len = 100 # How many points to show on screen
        # Deque automatically maintains fixed size history
        self.data_history = collections.deque([0.0] * self.data_history_len, maxlen=self.data_history_len) 

        self.create_widgets()
        self.setup_canvas()
        self.protocol("WM_DELETE_WINDOW", self.on_close)
        self.after(100, self.poll_queues) 

    def create_widgets(self):
        main_frame = tk.Frame(self)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        button_frame = tk.Frame(main_frame)
        button_frame.pack(fill=tk.X, pady=(0, 10))

        self.start_sender_btn = tk.Button(button_frame, text="Start Sender", command=lambda: self.handler.start_process('sender'))
        self.start_sender_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.start_receiver_btn = tk.Button(button_frame, text="Start Receiver (Scope View)", command=lambda: self.handler.start_process('receiver'))
        self.start_receiver_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.stop_btn = tk.Button(button_frame, text="Stop Process", command=self.handler.stop_process)
        self.stop_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)
        
        plot_container_frame = tk.Frame(main_frame, height=self.canvas_height + self.padding, bg='white')
        plot_container_frame.pack(fill=tk.X, pady=5)
        
        self.canvas = tk.Canvas(plot_container_frame, bg='black', height=self.canvas_height, width=self.canvas_width)
        self.canvas.pack(padx=self.padding/2, pady=self.padding/2)

        log_frame = tk.Frame(main_frame)
        log_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        self.log_ctrl = scrolledtext.ScrolledText(log_frame, state=tk.DISABLED, wrap=tk.WORD, bg='#333', fg='white')
        self.log_ctrl.pack(fill=tk.BOTH, expand=True)

        self.handler.log_message("GUI Initialized with oscilloscope view capability.", "System")
        
    def setup_canvas(self):
        """Draws initial grid and axes on the Tkinter canvas for oscilloscope feel."""
        # Draw center line (Y=0)
        center_y = self.canvas_height / 2
        self.canvas.create_line(0, center_y, self.canvas_width, center_y, fill="gray", dash=(4, 4), tags="grid")

        # Labels for Y-axis (example range -10 to 10)
        self.canvas.create_text(self.padding/2, self.padding, text="10V", fill="gray")
        self.canvas.create_text(self.padding/2, center_y, text="0V", fill="gray")
        self.canvas.create_text(self.padding/2, self.canvas_height - self.padding, text="-10V", fill="gray")

    def poll_queues(self):
        # Process log queue
        while True:
            try:
                message = self.log_queue.get_nowait()
                self.update_log_gui(message)
            except queue.Empty:
                break
        
        # Process data queue for plotting
        while True:
            try:
                # Get the single float value (f1)
                new_value = self.data_queue.get_nowait()
                self.update_canvas_oscilloscope(new_value)
            except queue.Empty:
                break

        self.after(50, self.poll_queues) # Poll faster for smoother graph

    def update_log_gui(self, message):
        self.log_ctrl.config(state=tk.NORMAL)
        self.log_ctrl.insert(tk.END, message)
        self.log_ctrl.see(tk.END)
        self.log_ctrl.config(state=tk.DISABLED)

    def update_canvas_oscilloscope(self, new_value):
        """Updates the Tkinter Canvas with a scrolling oscilloscope view."""
        self.data_history.append(new_value)
        self.canvas.delete("waveform") # Clear previous waveform

        # Define the graphing area boundaries
        graph_width = self.canvas_width
        graph_height_px = self.canvas_height
        
        # Assume data range is -10 to 10 (total range 20)
        min_val = -10.0
        max_val = 10.0
        val_range = max_val - min_val

        if val_range == 0: return

        points_coords = []

        # Iterate over the history and calculate coordinates
        for i, value in enumerate(self.data_history):
            # X position scales linearly across the canvas width
            x_pos = (i / (self.data_history_len - 1)) * graph_width
            # Y position scaled (inverted for Tkinter canvas)
            y_pos = graph_height_px - ((value - min_val) / val_range) * graph_height_px
            
            points_coords.append((x_pos, y_pos))

        # Draw the line connecting the historical points
        if len(points_coords) > 1:
            # Use 'green' for a classic oscilloscope look
            self.canvas.create_line(points_coords, fill="lime green", smooth=True, tags="waveform", width=2)


    def on_close(self):
        self.handler.stop_process()
        self.destroy()

if __name__ == "__main__":
    app = MainGUI()
    app.mainloop()
