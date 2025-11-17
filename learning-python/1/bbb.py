import socket
import serial
import time
import sys
import threading
import tkinter as tk
from tkinter import scrolledtext, messagebox
import queue
import collections
import math 
import copy

UDP_IP = "127.0.0.1"
UDP_PORT = 5005
SERIAL_PORT = '/dev/ttyUSB0' 
BAUD_RATE = 9600

class DataHandler:
    """Class to manage data transfer functions for a single window instance."""
    def __init__(self, log_queue, data_queue):
        self.log_queue = log_queue
        self.data_queue = data_queue 
        self.ser = None 
        self.stop_event = threading.Event()
        self.udp_sock_send = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.receiver_socket = None 

    def log_message(self, message, source="Info"):
        self.log_queue.put(f"[{source}] {message}\n")

    def cleanup_sockets_and_serial(self):
        """Helper to close all potential I/O sources."""
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
        if self.ser and self.ser.isOpen():
            try:
                self.ser.close()
            except serial.SerialException:
                pass

    def parse_and_enqueue_data(self, raw_data, source_name):
        """Central parsing logic used by receiver threads."""
        try:
            message_str = raw_data.decode('utf-8').strip()
            float_values = [float(x.strip()) for x in message_str.split(',')]
            
            if len(float_values) == 4:
                self.data_queue.put(float_values)
        except ValueError as e:
            self.log_message(f"Error parsing {source_name} floats: {e}", "Error")

    def udp_receiver_thread(self):
        """Listens for and parses UDP packets in a separate thread."""
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
                self.receiver_socket.settimeout(0.1) 
                data, addr = self.receiver_socket.recvfrom(1024)
                self.parse_and_enqueue_data(data, "UDP")
            except socket.timeout:
                continue
            except Exception as e:
                if not self.stop_event.is_set():
                    break
        self.cleanup_sockets_and_serial()
        self.log_message("UDP Receiver stopped.", "System")

    def serial_receiver_thread(self):
        """Reads from serial port, parses, and sends data to plot via queue."""
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) 
            self.log_message(f"Serial port {SERIAL_PORT} opened for receiving.", "Serial Info")
            
            while not self.stop_event.is_set():
                if self.ser.in_waiting > 0:
                    serial_line = self.ser.readline()
                    self.parse_and_enqueue_data(serial_line, "Serial")
        
        except serial.SerialException as e:
            self.log_message(f"Error opening serial port: {e}", "Error")
            self.log_message("Please check the SERIAL_PORT variable and ensure pyserial is installed.", "Error")
        except Exception as e:
            if not self.stop_event.is_set():
                pass
        finally:
            self.cleanup_sockets_and_serial()
            self.log_message("Serial Receiver stopped.", "System")

    def udp_serial_sender_thread(self):
        """Reads from serial port, parses, *plots*, and sends via UDP in a separate thread."""
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.5)
            self.log_message(f"Serial port {SERIAL_PORT} opened for sending.", "Serial Info")
            self.log_message(f"Sending UDP to {UDP_IP}:{UDP_PORT}", "UDP Send")
            
            while not self.stop_event.is_set():
                if self.ser.in_waiting > 0:
                    serial_line = self.ser.readline().decode('utf-8').strip()
                    try:
                        float_values = [float(x.strip()) for x in serial_line.split(',')]

                        if len(float_values) == 4:
                            self.data_queue.put(float_values) 

                            udp_message = ",".join(map(str, float_values))
                            self.udp_sock_send.sendto(bytes(udp_message, "utf-8"), (UDP_IP, UDP_PORT))
                            self.log_message(f"Sent UDP: {udp_message}", "UDP Send")
                    except ValueError as e:
                        self.log_message(f"Error parsing serial data: {e}", "Error")

        except serial.SerialException as e:
            self.log_message(f"Error opening serial port: {e}", "Error")
        except Exception as e:
            if not self.stop_event.is_set():
                pass
        finally:
            self.cleanup_sockets_and_serial()
            self.log_message("Sender background process finished.", "System")


    def start_process(self, process_type):
        if not self.stop_event.is_set():
            messagebox.showwarning("Warning", "A process is already running. Please stop it first.")
            return
        
        self.stop_event.clear()
        self.udp_sock_send = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        thread_map = {
            'udp_sender': self.udp_serial_sender_thread,
            'udp_receiver': self.udp_receiver_thread,
            'serial_receiver': self.serial_receiver_thread
        }
        
        thread_func = thread_map.get(process_type)

        if thread_func:
            data_thread = threading.Thread(target=thread_func, daemon=True)
            data_thread.start()

    def stop_process(self):
        self.log_message("Stop command issued...", "System")
        self.stop_event.set()
        self.cleanup_sockets_and_serial()


class MainGUI(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.master.title("UDP/Serial Interface GUI (Oscilloscope + Ellipse View)")
        self.master.geometry("600x600")
        
        self.log_queue = queue.Queue()
        self.data_queue = queue.Queue() 
        self.handler = DataHandler(self.log_queue, self.data_queue)
        
        self.canvas_height = 250
        self.canvas_width = 580
        self.padding = 30 
        self.data_history_len = 100 
        self.data_history = collections.deque([0.0] * self.data_history_len, maxlen=self.data_history_len) 
        
        self.selected_plot_value = tk.IntVar(value=0) 

        self.pack(fill=tk.BOTH, expand=True)
        self.create_widgets()
        self.setup_canvas()
        self.master.protocol("WM_DELETE_WINDOW", self.on_close) 
        self.after(50, self.poll_queues) 
        
        self.Y_MIN = 0.0
        self.Y_MAX = 1000.0
        self.LOG_Y_MIN = math.log10(self.Y_MIN + 1)
        self.LOG_Y_MAX = math.log10(self.Y_MAX + 1)
        
        self.F_MIN_VAL = 0.0
        self.F_MAX_VAL = 1000.0

    def create_widgets(self):
        main_frame = self
        
        button_frame = tk.Frame(main_frame)
        button_frame.pack(fill=tk.X, pady=(0, 10))

        self.start_sender_btn = tk.Button(button_frame, text="Start Serial->UDP Sender", command=lambda: self.handler.start_process('udp_sender'))
        self.start_sender_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.start_udp_receiver_btn = tk.Button(button_frame, text="Start UDP Receiver (Scope/Ellipse)", command=lambda: self.handler.start_process('udp_receiver'))
        self.start_udp_receiver_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)
        
        self.start_serial_receiver_btn = tk.Button(button_frame, text="Start Serial Receiver (Scope/Ellipse)", command=lambda: self.handler.start_process('serial_receiver'))
        self.start_serial_receiver_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.stop_btn = tk.Button(button_frame, text="Stop Process", command=self.handler.stop_process, bg='red', fg='white')
        self.stop_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)
        
        plot_selection_frame = tk.Frame(main_frame)
        plot_selection_frame.pack(fill=tk.X, pady=(0, 5))

        tk.Label(plot_selection_frame, text="Select Value to Plot (Oscilloscope, 0-1000 log scale):").pack(side=tk.LEFT, padx=(5, 10))
        tk.Radiobutton(plot_selection_frame, text="f1", variable=self.selected_plot_value, value=0).pack(side=tk.LEFT, padx=5)
        tk.Radiobutton(plot_selection_frame, text="f2", variable=self.selected_plot_value, value=1).pack(side=tk.LEFT, padx=5)
        tk.Radiobutton(plot_selection_frame, text="f3", variable=self.selected_plot_value, value=2).pack(side=tk.LEFT, padx=5)
        tk.Radiobutton(plot_selection_frame, text="f4", variable=self.selected_plot_value, value=3).pack(side=tk.LEFT, padx=5)
        
        plot_container_frame = tk.Frame(main_frame, height=self.canvas_height + self.padding, bg='white')
        plot_container_frame.pack(fill=tk.X, pady=5)
        
        self.canvas = tk.Canvas(plot_container_frame, bg='black', height=self.canvas_height, width=self.canvas_width)
        self.canvas.pack(padx=self.padding/2, pady=self.padding/2)

        log_frame = tk.Frame(main_frame)
        log_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        self.log_ctrl = scrolledtext.ScrolledText(log_frame, state=tk.DISABLED, wrap=tk.WORD, bg='#333', fg='white')
        self.log_ctrl.pack(fill=tk.BOTH, expand=True)

        self.handler.log_message("GUI Initialized with overlayed plots. Ellipse position is now proportional to selected value.", "System")
        
    def setup_canvas(self):
        # Y=0 line (bottom) - reference line
        self.canvas.create_line(0, self.canvas_height - 1, self.canvas_width, self.canvas_height - 1, fill="gray", dash=(4, 4), tags="grid")
        self.canvas.create_text(self.padding/2, self.canvas_height - 10, text="0", fill="gray")

        # Y=1000 line (top) - reference line
        self.canvas.create_line(0, 1, self.canvas_width, 1, fill="gray", dash=(4, 4), tags="grid")
        self.canvas.create_text(self.padding/2, 10, text="1000", fill="gray")

        # Example intermediate labels (approximate log positions)
        self.canvas.create_text(self.padding/2, self.canvas_height * 0.75, text="10", fill="gray") 
        self.canvas.create_text(self.padding/2, self.canvas_height * 0.45, text="100", fill="gray")

    def poll_queues(self):
        while True:
            try:
                message = self.log_queue.get_nowait()
                self.update_log_gui(message)
            except queue.Empty:
                break
        
        while True:
            try:
                all_values = self.data_queue.get_nowait()
                if len(all_values) == 4:
                    plot_index = self.selected_plot_value.get()
                    value_to_plot = all_values[plot_index]
                    
                    # Update oscilloscope plot data
                    value_to_plot_scope = max(0.0, value_to_plot) 
                    self.update_canvas_oscilloscope(value_to_plot_scope)

                    # Update ellipse plot data
                    self.update_canvas_ellipse(value_to_plot)
            except queue.Empty:
                break

        self.after(50, self.poll_queues)

    def update_log_gui(self, message):
        self.log_ctrl.config(state=tk.NORMAL)
        self.log_ctrl.insert(tk.END, message)
        self.log_ctrl.see(tk.END)
        self.log_ctrl.config(state=tk.DISABLED)
        
    def log_scale(self, value):
        """Converts a linear value (0-1000) to a position on the canvas's log Y-axis."""
        log_value = math.log10(value + 1)
        log_range = self.LOG_Y_MAX - self.LOG_Y_MIN

        if log_range == 0: return self.canvas_height
        normalized_log_value = (log_value - self.LOG_Y_MIN) / log_range
        y_pos = self.canvas_height * (1 - normalized_log_value)
        return y_pos

    def update_canvas_oscilloscope(self, new_value):
        """Updates the Tkinter Canvas with a scrolling oscilloscope view."""
        self.data_history.append(new_value)
        self.canvas.delete("waveform") 
        points_coords = []
        for i, value in enumerate(self.data_history):
            x_pos = (i / (self.data_history_len - 1)) * self.canvas_width
            y_pos = self.log_scale(value) 
            points_coords.append((x_pos, y_pos))

        if len(points_coords) > 1:
            self.canvas.create_line(points_coords, fill="lime green", smooth=True, tags="waveform", width=2)

    def update_canvas_ellipse(self, value_to_plot):
        """Updates the Tkinter Canvas with a horizontally moving ellipse, overlaid."""
        self.canvas.delete("ellipse")

        # Clamp the value to the defined range (0-1000)
        clamped_value = max(self.F_MIN_VAL, min(self.F_MAX_VAL, value_to_plot))
        
        ellipse_width = 30
        
        # Calculate the proportional X position of the ellipse's left edge
        # The total movement range is the canvas width minus the ellipse's own width
        x_range = self.canvas_width - ellipse_width
        
        # Calculate the scaled X position
        scaled_x = (clamped_value / self.F_MAX_VAL) * x_range
        scaled_x = (clamped_value / 50.0)*self.canvas_width
        x1 = scaled_x
        x2 = scaled_x + ellipse_width
        
        y_center = self.canvas_height / 2
        y_radius = 20
        y1 = y_center - y_radius
        y2 = y_center + y_radius
        
        self.canvas.create_oval(x1, y1, x2, y2, fill="yellow", outline="orange", width=2, tags="ellipse")


    def on_close(self):
        self.handler.stop_process()
        self.master.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = MainGUI(root)
    root.mainloop()
