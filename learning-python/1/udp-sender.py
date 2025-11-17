import socket
import serial
import time
import sys
import threading
import tkinter as tk
from tkinter import scrolledtext, messagebox
import queue

UDP_IP = "127.0.0.1"
UDP_PORT = 5005
SERIAL_PORT = '/dev/ttyUSB0' # Change this to your serial port (e.g., 'COM3' on Windows)
BAUD_RATE = 9600

class DataHandler:
    """Class to manage data transfer functions."""
    def __init__(self, log_queue):
        self.log_queue = log_queue
        self.ser = None
        self.udp_sock_send = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # Sockets for receiving need to be bound in the thread that uses them, after the stop event is clear.
        self.stop_event = threading.Event()
        self.receiver_socket = None

    def log_message(self, message, source="Info"):
        """Thread-safe way to put messages into the queue for the GUI."""
        self.log_queue.put(f"[{source}] {message}\n")

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
                self.receiver_socket.settimeout(0.5) 
                data, addr = self.receiver_socket.recvfrom(1024)
                message_str = data.decode('utf-8').strip()
                
                self.log_message(f"Received UDP: {message_str}", "UDP Recv")
                
                float_values = [float(x.strip()) for x in message_str.split(',')]
                
                if len(float_values) == 4:
                    f1, f2, f3, f4 = float_values
                    self.log_message(f"Parsed floats: f1={f1}, f2={f2}, f3={f3}, f4={f4}", "UDP Parse")
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

    def udp_serial_sender_thread(self):
        """Reads from serial port, parses, and sends via UDP in a separate thread."""
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
            self.udp_sock_send.close()
        if self.receiver_socket:
            self.receiver_socket.close()

    def start_process(self, process_type):
        """Starts the specified data handling thread."""
        if not self.stop_event.is_set():
            messagebox.showwarning("Warning", "A process is already running. Please stop it first.")
            return
        
        self.stop_event.clear()
        # Re-initialize sockets if they were closed during a previous stop
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
        """Signals the background threads to stop."""
        self.log_message("Stop command issued...", "System")
        self.stop_event.set()
        # Closing the sockets here helps break blocking I/O calls in the threads
        self.cleanup_sockets()


class MainGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("UDP/Serial Interface GUI (Tkinter)")
        self.geometry("600x400")
        
        # Queue for thread-safe logging
        self.log_queue = queue.Queue()
        self.handler = DataHandler(self.log_queue)

        self.create_widgets()
        self.protocol("WM_DELETE_WINDOW", self.on_close)
        self.after(100, self.poll_log_queue) # Start polling the queue

    def create_widgets(self):
        main_frame = tk.Frame(self)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Buttons Frame
        button_frame = tk.Frame(main_frame)
        button_frame.pack(fill=tk.X, pady=(0, 10))

        self.start_sender_btn = tk.Button(button_frame, text="Start Sender (Serial->UDP)", command=lambda: self.handler.start_process('sender'))
        self.start_sender_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.start_receiver_btn = tk.Button(button_frame, text="Start Receiver (UDP->Console)", command=lambda: self.handler.start_process('receiver'))
        self.start_receiver_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        self.stop_btn = tk.Button(button_frame, text="Stop Process", command=self.handler.stop_process)
        self.stop_btn.pack(side=tk.LEFT, expand=True, fill=tk.X, padx=5)

        # Log Area
        self.log_ctrl = scrolledtext.ScrolledText(main_frame, state=tk.DISABLED, wrap=tk.WORD)
        self.log_ctrl.pack(fill=tk.BOTH, expand=True)

        self.handler.log_message("GUI Initialized. Select an option.", "System")

    def poll_log_queue(self):
        """Check the queue for new messages and update the log area."""
        while True:
            try:
                message = self.log_queue.get_nowait()
                self.update_log_gui(message)
            except queue.Empty:
                break
        self.after(100, self.poll_log_queue) # Check again in 100ms

    def update_log_gui(self, message):
        """Appends a message to the Tkinter Text widget."""
        self.log_ctrl.config(state=tk.NORMAL)
        self.log_ctrl.insert(tk.END, message)
        self.log_ctrl.see(tk.END) # Auto-scroll to the bottom
        self.log_ctrl.config(state=tk.DISABLED)

    def on_close(self):
        """Handles the window close event."""
        self.handler.stop_process() # Ensure threads are terminated
        self.destroy()

if __name__ == "__main__":
    app = MainGUI()
    app.mainloop()
