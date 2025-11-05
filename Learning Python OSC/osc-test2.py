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
    serial_client = None  # Ensure serial_client is None if connection fails

# Updated keypad layout with a total of 20 keys
keys = [
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
    '^', 'G', 'H', 'E',
    'v', '<', '>', 'F'
]

def send_osc(key):
    """Send an OSC message when a key is pressed."""
    osc_address = f"/key/{key}"
    try:
        osc_client.send_message(osc_address, 1)
        print(f"Sent OSC message to {osc_address}")
    except Exception as e:
        pass  # Handle any OSC send exceptions gracefully
        
def send_serial(key):
    """Send a serial message when a key is pressed."""
    if serial_client:
        message = f"/key/{key}\n"  # Assuming message ends with newline
        try:
            serial_client.write(message.encode('utf-8'))
            print(f"Sent message to serial: {message.strip()}")
        except Exception as e:
            print(f"Failed to send message to serial: {e}")

def create_button(key, row, col):
    """Create a button for the keypad."""
    def on_button_press():
        original_color = button.cget('bg')
        button.configure(bg='lightblue')  # Change color on press
        # Call both OSC and serial functions
        send_osc(key)
        send_serial(key)
        button.after(100, lambda: button.configure(bg=original_color))  # Reset color after 100ms

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
root.title("Arduino 4x6 Keypad")
root.geometry("500x500")  # Set an appropriate window size
root.configure(bg='lightgray')

# Create buttons dynamically in a grid
for idx, key in enumerate(keys):
    create_button(key, idx // 4, idx % 4)

# Run the application
try:
    root.mainloop()
finally:
    if serial_client and serial_client.is_open:
        serial_client.close()  # Ensure the serial connection is closed on exit
