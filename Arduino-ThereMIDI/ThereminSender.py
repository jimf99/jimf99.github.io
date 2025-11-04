import time
import PySimpleGUI as sg
from pythonosc import udp_client

# Define the OSC client
client = udp_client.SimpleUDPClient("127.0.0.1", 4560)

import math

def midi_to_hz(midi_note):
    if not 0 <= midi_note <= 127:
        return 0
    a4_freq = 440.0
    a4_midi = 69
    return a4_freq * math.pow(2, (midi_note - a4_midi) / 12.0)

buttonName=[0,'1','2','3','A','4','5','6','B',7,8,9,'C','*','0','#','D']

# Define the GUI layout
layout = [
    [sg.Text("PyTheremin Controller", font=("Helvetica", 16))],
    [sg.Text("Note", size=(10, 1)), sg.Slider(range=(0,100), orientation='h', size=(30, 20), key='note', default_value=50)],
    [sg.Text("Amplitude", size=(10, 1)), sg.Slider(range=(0, 100), orientation='h', size=(30, 20), key='amp', default_value=0.5, resolution=0.01)],
    [[sg.Button(f'{buttonName[row*4+(col+1)]}', size=(1, 1), key=buttonName[row*4+(col+1)]) for col in range(4)] for row in range(4)],
]

# Create the window
window = sg.Window("Theremin Control", layout, finalize=True)

prevSlider1=-1;
# Event loop
while True:
    event1, values = window.read(timeout=20)
    
    if event1 == sg.WINDOW_CLOSED or event1 == "Exit":
        break

  # Handle button clicks
    if not(event1=="__TIMEOUT__") :
        if(event1 in buttonName) :
            print(event1)
        slider1=-1
        
  # Get slider values
    slider1=values['note']
    amp = values['amp']
    if not(slider1 == prevSlider1) :
        pitch= midi_to_hz(slider1+(69-8))
        pitch=round(pitch,4)
    pitch=slider1
    
    # Send OSC message to Sonic Pi
    print(pitch,amp)
    client.send_message("/theremin/control", [pitch, amp])
    prevSlider1 = slider1
    
# Close the window
window.close()

"""
# Sonic Pi Theremin Receiver
# This code listens for OSC messages from the Python GUI.

play 69
sleep(1)
play 70
sleep(1)
play 71
sleep(3)

live_loop :theremin_receiver do
  # get OSC message with the address "/theremin/control".
  osc_data = sync "/osc*/theremin/control"
  val1 = osc_data[0]
  val2 = osc_data[1]/100
  n=val1
  a=val2
  use_synth :piano
  play note: n, amp: a, attack: 0.1, decay: 0.1, sustain: 0.2
end
"""
