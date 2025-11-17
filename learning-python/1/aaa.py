import serial
import threading
import sys
import time
import struct

# --- Configuration ---
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
TIMEOUT = 1

# ---------------- OSC PACKING FUNCTIONS ----------------

def pad_osc_string(s: str) -> bytes:
    """Pad OSC string to multiple of 4 bytes."""
    data = s.encode('utf-8') + b'\x00'
    padding = (4 - (len(data) % 4)) % 4
    return data + (b'\x00' * padding)

def pad_osc_blob(b: bytes) -> bytes:
    """Pad blob to multiple of 4 bytes."""
    padding = (4 - (len(b) % 4)) % 4
    return b + (b'\x00' * padding)

def build_osc_message(address: str, values):
    """Build a binary OSC message with 4 float values."""
    if len(values) != 4:
        raise ValueError("OSC requires exactly 4 values")

    # Address string
    packet = pad_osc_string(address)

    # Type tag string: 4 floats → ",ffff"
    type_tag = "," + ("f" * 4)
    packet += pad_osc_string(type_tag)

    # Append floats
    for val in values:
        packet += struct.pack(">f", float(val))  # OSC = big-endian
        print(packet)

    return packet

# ---------------- SERIAL FUNCTIONS ----------------

def read_from_port(ser):
    while True:
        try:
            raw_line = ser.readline()
            if raw_line:
                decoded_line = raw_line.decode('utf-8', errors="replace").strip()
                if decoded_line:
                    print(f"{decoded_line}")
        except serial.SerialException as e:
            print(f"Device disconnected: {e}")
            break
        except Exception as e:
            print(f"Error reading from port: {e}")
            break

def write_to_port(ser):
    """Detect 'osc' commands and send binary OSC messages."""
    while True:
        try:
            user_input = input("")

            # --- OSC command parser ---
            if user_input.startswith("osc "):
                try:
                    parts = user_input.split()
                    address = parts[1]
                    values = parts[2:6]  # Expect 4 numbers

                    if len(values) != 4:
                        print("Error: OSC requires exactly 4 numerical values.")
                        continue

                    osc_packet = build_osc_message(address, values)

                    ser.write(osc_packet)
                    print(f"Sent OSC: {address} {values}")
                except Exception as e:
                    print(f"OSC error: {e}")
                continue

            # --- Default: send as text ---
            ser.write(str.encode(user_input + "\n"))

        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Error writing to port: {e}")
            break

def run_terminal():
    try:
        ser = serial.Serial(
            port=SERIAL_PORT,
            baudrate=BAUD_RATE,
            timeout=TIMEOUT
        )
        time.sleep(2) 
        ser.flushInput()

        print(f"--- Connected to {SERIAL_PORT} at {BAUD_RATE} baud ---")
        print("Type 'osc /addr v1 v2 v3 v4' to send OSC messages.")
        print("Type normal text to send raw serial data.")

        read_thread = threading.Thread(target=read_from_port, args=(ser,), daemon=True)
        read_thread.start()

        write_to_port(ser)

    except serial.SerialException as e:
        print(f"Error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nExiting terminal.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed.")

if __name__ == "__main__":
    run_terminal()
