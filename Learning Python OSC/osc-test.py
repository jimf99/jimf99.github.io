import tkinter as tk
from pythonosc import udp_client

# Set up the OSC client
ip = "127.0.0.1"  # Change this to your target IP
port = 4560       # OSC port
client = udp_client.SimpleUDPClient(ip, port)

# Keypad layout similar to Arduino (1, 2, 3, A, ...)
keys = [
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
]

# Function to send OSC command
def send_osc(key):
    osc_address = f"/key/{key}"
    client.send_message(osc_address, 1)
    print(f"Sent OSC message to {osc_address}")

# Create the main window
root = tk.Tk()
root.title("Arduino 4x4 Keypad")
root.geometry("400x400")  # Set a fixed window size
root.configure(bg='lightgray')

# Create buttons similarly to an Arduino 4x4 keypad
def create_button(key, row, col):
    button = tk.Button(root, text=key,
                       font=('Arial', 24, 'bold'), fg='blue',  # Set text color to blue
                       command=lambda: send_osc(key),
                       bg='white', activebackground='lightgray',  # Button background
                       relief='raised', borderwidth=5)
    
    button.grid(row=row, column=col, padx=10, pady=10, sticky='nsew')

    # Make buttons expand in the grid
    root.grid_rowconfigure(row, weight=1)
    root.grid_columnconfigure(col, weight=1)

# Create buttons dynamically in a grid
for idx, key in enumerate(keys):
    create_button(key, idx // 4, idx % 4)

# Run the application
root.mainloop()
