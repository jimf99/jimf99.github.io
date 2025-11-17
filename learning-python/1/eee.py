import tkinter as tk
from tkinter import ttk
import os
import serial
import serial.tools.list_ports

# --- Configuration ---
SCALES = {
    "C Major (A3 start)": ["A3", "B3", "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"],
    "C Major (C4 start)": ["C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5", "D5", "E5"]
}
KEY_MAP = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0']
DEFAULT_SCALE = "C Major (A3 start)"
BUTTON_STYLE = {'width': 5, 'height': 5, 'bg': 'white', 'activebackground': 'red'}

# --- Global variables ---
current_scale_notes = SCALES[DEFAULT_SCALE]
piano_buttons = {}
last_note_index = None  # For theremin-like smoothness

# --- Serial Setup ---
SERIAL_PORT = "/dev/ttyUSB0"      # Change as needed
BAUD_RATE = 9600

ser = None
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
    print(f"Serial open on {SERIAL_PORT}")
except Exception as e:
    print("Serial unavailable:", e)


# --- Functions ---

def read_serial_floats():
    """Reads a serial line and returns a list of 4 floats."""
    if ser is None:
        return None

    try:
        line = ser.readline().decode().strip()
        if not line:
            return None

        parts = line.split(",")
        floats = [float(p) for p in parts]
        return floats

    except Exception as e:
        print("Serial error:", e)
        return None


def serial_poll():
    """Poll serial every 50 ms and map 3rd float (f3) to nearest note (theremin-like)."""
    global last_note_index

    vals = read_serial_floats()
    if vals:
        f1, f2, f3, f4 = vals
        status_label.config(text=f"Serial: {vals}")

        # Map f3 (0-100) to note index 0-9
        note_index = round(f3*10)
        note_index = min(note_index, 9)
        key_char = KEY_MAP[note_index]

        # Only trigger when entering a new note index (theremin-like smoothness)
        if note_index != last_note_index:
            event = type('Event', (object,), {'char': key_char})
            on_key_press(event)
            last_note_index = note_index

    root.after(250, serial_poll)


def play_sound(note):
    sound_file = f"{note}.wav"
    if os.path.exists(sound_file):
        try:
            print(f"Playing sound: {sound_file}")  # Placeholder
        except Exception as e:
            print(f"Error playing sound: {e}")
    else:
        print(f"Sound file not found: {sound_file}")


def highlight_key(key_char, color):
    if key_char in piano_buttons:
        piano_buttons[key_char].config(bg=color)
        if color == 'red':
            root.after(150, highlight_key, key_char, 'white')


def on_key_press(event):
    key = event.char
    if key in KEY_MAP:
        note_index = int(key) - 1 if key != '0' else 9
        note = current_scale_notes[note_index]
        display_note(note)
        highlight_key(key, 'red')
        play_sound(note)
    elif key in ('q', 'Q'):
        root.destroy()


def display_note(note):
    status_label.config(text=f"Last Note Played: {note}")


def change_scale(selection):
    global current_scale_notes
    current_scale_notes = SCALES[selection]
    display_note(f"Scale changed to {selection}")


# --- Main Application Setup ---
root = tk.Tk()
root.title("Enhanced Musical Keyboard (1-0 keys)")
root.geometry("400x350")
root.configure(bg='#f0f0f0')

# Dropdown Menu
scale_var = tk.StringVar(root)
scale_var.set(DEFAULT_SCALE)
scale_menu = ttk.Combobox(root, textvariable=scale_var, values=list(SCALES.keys()), state="readonly")
scale_menu.pack(pady=10)
scale_menu.bind("<<ComboboxSelected>>", lambda event: change_scale(scale_var.get()))

# Piano Key Visuals
key_frame = tk.Frame(root, bg='#f0f0f0')
key_frame.pack(pady=20)

for i, key_char in enumerate(KEY_MAP):
    note_name = SCALES[DEFAULT_SCALE][i]
    btn = tk.Button(
        key_frame,
        text=f"{key_char}\n({note_name})",
        **BUTTON_STYLE,
        command=lambda k=key_char: on_key_press(type('Event', (object,), {'char': k}))
    )
    btn.pack(side=tk.LEFT, padx=2, pady=2)
    piano_buttons[key_char] = btn

# Status Label
status_label = tk.Label(root, text="Press keys 1 through 0", font=("Helvetica", 16), bg='#f0f0f0')
status_label.pack(pady=10)

instruction_label = tk.Label(root, text="Press 'q' to quit", font=("Helvetica", 10), bg='#f0f0f0')
instruction_label.pack(pady=5)

root.bind("<Key>", on_key_press)
root.focus_set()

# Start serial polling
serial_poll()

# Launch Tkinter
root.mainloop()
