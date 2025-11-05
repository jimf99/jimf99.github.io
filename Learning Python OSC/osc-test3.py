import tkinter as tk
from pythonosc import udp_client
import serial
import time

# Constants for OSC configuration
OSC_IP = "127.0.0.1"  # Target IP for OSC
OSC_PORT = 4560       # OSC port

# Constants for Serial configuration
SERIAL_PORT = '/dev/cu.usbserial-0001'  # Change this to your USB serial port
BAUD_RATE = 9600                       # Standard baud rate

# Set up the OSC client
osc_client = udp_client.SimpleUDPClient(OSC_IP, OSC_PORT)

# Set up the serial client
try:
    serial_client = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for the connection to establish
except serial.SerialException as e:
    serial_client = None  # Ensure serial_client is None if the connection fails

# Keypad layout for Arduino 
keys = [
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
]

# Function keys with 'fn' prefix
function_keys = [
    'fn1', 'fn2', 'fn3', 'fn4',
    'fn5', 'fn6', 'fn7', 'fn8'
]

# Directional keys
directional_keys = ['UP', 'DOWN', 'LEFT', 'RIGHT']

def send_osc(key):
    """Send an OSC message when a key is pressed."""
    if key.startswith('fn'):
        osc_address = f"/fn/{key[2:]}"  # For function keys
    else:
        osc_address = f"/key/{key}"  # For numeric keys
    try:
        osc_client.send_message(osc_address, 1)
        print(f"Sent OSC message to {osc_address}")
    except Exception:
        pass

def send_serial(key):
    """Send a serial message when a key is pressed."""
    if serial_client:
        if key.startswith('fn'):
            message = f"/fn/{key[2:]}\n"
        else:
            message = f"/key/{key}\n"
        try:
            serial_client.write(message.encode('utf-8'))  # Send the message
            print(f"Sent message to serial: {message.strip()}")
        except Exception:
            pass

def create_button(key, row, col):
    """Create a button for the keypad."""
    def on_button_press():
        original_color = button.cget('bg')  # Get the original background color
        button.configure(bg='lightblue')  # Change color on press
        send_osc(key)
        send_serial(key)
        button.after(100, lambda: button.configure(bg=original_color))  # Reset color

    button = tk.Button(root, text=key,
                       font=('Arial', 24, 'bold'), fg='blue',
                       command=on_button_press,
                       bg='white', activebackground='lightgray',
                       relief='raised', borderwidth=5)

    button.grid(row=row, column=col, padx=10, pady=10, sticky='nsew')

    # Make buttons expand in the grid
    root.grid_rowconfigure(row, weight=1)
    root.grid_columnconfigure(col, weight=1)

# Create the main window
root = tk.Tk()
root.title("Arduino Keypad")
root.geometry("400x400")  # Set a fixed window size
root.configure(bg='lightgray')

# Create buttons for the 4x4 keypad
for idx, key in enumerate(keys):
    create_button(key, idx // 4, idx % 4)

# Create buttons for the function keys
for idx, key in enumerate(function_keys):
    create_button(key, idx // 4 + 4, idx % 4)  # Start from row 4 for the new buttons

# Create directional buttons in the same row, rightmost column
directional_row = 4  # All directional buttons in the same row
for idx, key in enumerate(directional_keys):
    create_button(key, directional_row, 4)  # Rightmost column (index 4)

# Run the application
try:
    root.mainloop()
finally:
    if serial_client and serial_client.is_open:
        serial
