import serial
import threading
import sys
import time

# --- Configuration ---
SERIAL_PORT = '/dev/ttyUSB0' # Change to your port ('/dev/ttyUSB0', 'COM1', etc.)
BAUD_RATE = 9600
TIMEOUT = 1

def read_from_port(ser):
    """Continuously read data from the serial port and print it."""
    while True:
        try:
            # Read a line and decode it
            raw_line = ser.readline()
            if raw_line:
                decoded_line = raw_line.decode('utf-8').strip()
                if decoded_line:
                    items=decoded_line.split(",")
                    items[0]=float(items[0])
                    items[1]=float(items[1])
                    items[2]=float(items[2])
                    items[3]=float(items[3])
                    print(items)
                    ## print(f"{decoded_line}")
        except serial.SerialException as e:
            print(f"Device disconnected: {e}")
            break
        except Exception as e:
            print(f"Error reading from port: {e}")
            break

def write_to_port(ser):
    """Read input from the keyboard and write it to the serial port."""
    while True:
        try:
            # Get user input
            user_input = input("")
            # Encode input to bytes and add a newline character
            ser.write(str.encode(user_input + '\n'))
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Error writing to port: {e}")
            break

def run_terminal():
    """Main function to set up and run the serial terminal."""
    try:
        ser = serial.Serial(
            port=SERIAL_PORT,
            baudrate=BAUD_RATE,
            timeout=TIMEOUT
        )
        time.sleep(2) # Wait for the connection to establish
        ser.flushInput()
        print(f"--- Connected to {SERIAL_PORT} at {BAUD_RATE} baud. Type 'exit' to quit. ---")

        # Start a separate thread for reading from the serial port
        read_thread = threading.Thread(target=read_from_port, args=(ser,), daemon=True)
        read_thread.start()

        # Use the main thread for writing to the serial port
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
