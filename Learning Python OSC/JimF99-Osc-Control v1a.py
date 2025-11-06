import tkinter as tk
import serial
import time
import threading

# Configure your serial port settings
SERIAL_PORT = '/dev/ttyUSB0'  # Change this to your serial port
BAUD_RATE = 9600               # Adjust baud rate if necessary
TIMEOUT = 1       

import argparse
import random
import time


class KeypadApp:
    def __init__(self, root):
        self.root = root
        self.root.title("4x4 Keypad")

        # Initialize serial connection
        self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Define button values
        self.keys = [
            ['1', '2', '3', 'A','^'],
            ['4', '5', '6', 'B','v'],
            ['7', '8', '9', 'C','<'],
            ['*', '0', '#', 'D','>']
        ]

        # Colors
        button_color = "blue"
        text_color = "white"
        special_color = "red"

        # Create buttons
        for r, row in enumerate(self.keys):
            for c, key in enumerate(row):
                if key in ['A', 'B', 'C', 'D']:
                    btn = tk.Button(self.root, text=key, bg=special_color, fg=text_color, 
                                    font=('Arial', 20), width=4, height=2,
                                    command=lambda k=key: self.on_button_press(k))
                else:
                    btn = tk.Button(self.root, text=key, bg=button_color, fg=text_color, 
                                    font=('Arial', 20), width=4, height=2,
                                    command=lambda k=key: self.on_button_press(k))

                btn.grid(row=r, column=c, padx=5, pady=5)

        # Start the serial listening thread
        self.listening_thread = threading.Thread(target=self.listen_to_serial, daemon=True)
        self.listening_thread.start()

    def listen_to_serial(self):
        while self.ser.is_open:
            if self.ser.in_waiting > 0:
                incoming_data = self.ser.readline().decode('utf-8').strip()
                print(f'Received: {incoming_data}')
                # Update GUI safely
                self.root.after(0, self.update_gui, incoming_data)

    def update_gui(self, data):
        # This method can be used to update your Tkinter GUI with the received data
        # print(f'Updating GUI with: {data}')  # For example, you could display this data in a label.
        {}
        
    def on_button_press(self, key):
        # print(f'Button {key} pressed.')
        self.send_to_serial(key)

    def send_to_serial(self, key):
        if self.ser.is_open:
            # Send key as bytes
            toSend="/keypad/"+str(key)+"\r"
            self.ser.write(toSend.encode('utf-8'))
            # print(f'Sent "{toSend}" to serial.')

    def close_serial(self):
        # Close the serial connection when the application is closed
        if self.ser.is_open:
            self.ser.close()
            print(f"Disconnected from {SERIAL_PORT}.")

if __name__ == "__main__":
    root = tk.Tk()
    app = KeypadApp(root)
    
    # Properly close serial on window close
    # root.protocol("WM_DELETE_WINDOW", app.close_serial)
    
    root.mainloop()
