 Python Theremin GUI
# This script sends OSC messages to Sonic Pi.

import PySimpleGUI as sg
from pythonosc import udp_client

# Define the OSC client
client = udp_client.SimpleUDPClient("127.0.0.1", 4560)

# Define the GUI layout
layout = [
    [sg.Text("PyTheremin Controller", font=("Helvetica", 16))],
    [sg.Text("Pitch", size=(10, 1)), sg.Slider(range=(40, 100), orientation='h', size=(30, 20), key='pitch', default_value=60)],
    [sg.Text("Amplitude", size=(10, 1)), sg.Slider(range=(0, 1), orientation='h', size=(30, 20), key='amp', default_value=0.5, resolution=0.01)],
    [sg.Button("Exit")]
]

# Create the window
window = sg.Window("Theremin Control", layout, finalize=True)

# Event loop
while True:
    event, values = window.read(timeout=20) # Read events every 20ms
    
    # Exit condition
    if event == sg.WINDOW_CLOSED or event == "Exit":
        break

    # Get slider values
    pitch = int(values['pitch'])
    amp = values['amp']

    # Send OSC message to Sonic Pi
    client.send_message("/theremin/control", [pitch, amp])

# Close the window
window.close()
