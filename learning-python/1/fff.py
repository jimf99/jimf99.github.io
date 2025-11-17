import numpy as np
from scipy.io.wavfile import write
import math

# Sample rate (standard for CD quality audio)
samplerate = 44100

def generate_note_frequency(note_name):
    """
    Calculates the frequency (Hz) of a given musical note.
    Using A4 (440 Hz) as the reference point.
    """
    # Define reference A4 = 440 Hz
    A4_freq = 440.0
    notes_order = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]

    # Parse the note name (e.g., "A3" -> "A", 3)
    # This simple parser assumes only flat names, but our notes are all natural
    note_letter = note_name[0]
    octave = int(note_name[-1])

    # Find the position of the note within the octave
    note_index = notes_order.index(note_letter)

    # Calculate distance in semitones from A4 (A4 is 57th semitone from C0 if C0 is 0)
    # A4 is index 9 in notes_order.
    distance_semitones = (octave - 4) * 12 + (note_index - 9)
    frequency = A4_freq * (2 ** (distance_semitones / 12.0))
    return frequency

def create_sine_wave(freq, duration_seconds=0.5):
    """Generates a sine wave for the given frequency and duration."""
    t = np.linspace(0., duration_seconds, int(samplerate * duration_seconds), endpoint=False)
    # Generate sine wave: amplitude * sin(2 * pi * frequency * time)
    # Amplitude is scaled down (0.3) to avoid clipping and keep volume reasonable
    amplitude = 0.3
    waveform = amplitude * np.sin(2. * math.pi * freq * t)
    
    # Apply a simple fade out at the end to prevent clicking sound
    fade_out_duration = 0.05 # 50 ms
    fade_out_samples = int(fade_out_duration * samplerate)
    if fade_out_samples < len(waveform):
        fade_curve = np.linspace(1., 0., fade_out_samples)
        waveform[-fade_out_samples:] *= fade_curve
        
    return waveform

def generate_files():
    """Generates and saves all required WAV files."""
    notes_list = ["A3", "B3", "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"]
    duration = 0.3 # Shorter duration for piano keys response

    print(f"Generating {len(notes_list)} sound files...")

    for note in notes_list:
        freq = generate_note_frequency(note)
        audio_data = create_sine_wave(freq, duration)

        # Convert to 16-bit integers (standard WAV format)
        # Scale the data range to fit within int16 limits
        audio_data_int16 = (audio_data * 32767).astype(np.int16)

        filename = f"{note}.wav"
        write(filename, samplerate, audio_data_int16)
        print(f"Created {filename} at {freq:.2f} Hz")
        
    print("Generation complete. Files are ready in the current directory.")

if __name__ == "__main__":
    generate_files()
