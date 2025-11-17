import serial
import time
import collections

# --- Configuration ---
# Change 'COM3' to your port name (e.g., '/dev/ttyUSB0' on Linux, 'COM3' on Windows)
SERIAL_PORT = '/dev/ttyUSB0' 
BAUD_RATE = 9600
NUM_VALUES = 4
SMOOTHING_WINDOW_SIZE = 10 # Reverted to a smaller window size for less aggressive smoothing
F3_INDEX = 2 # Index for the third float (0-indexed list)

def calculate_rolling_average(new_value, buffer):
    """Adds a new value to the buffer and calculates the current average."""
    buffer.append(new_value)
    return sum(buffer) / len(buffer)

def parse_and_process_line(line, buffers):
    """
    Parses a line of serial data into floats, smooths them, 
    and returns a list of rounded smoothed values.
    """
    try:
        values = [float(val) for val in line.split(',')]
        
        if len(values) == NUM_VALUES:
            smoothed_values = []
            for i, value in enumerate(values):
                # Apply smoothing
                smoothed_val = calculate_rolling_average(value, buffers[i])
                # Round to 1 decimal place
                rounded_val = round(smoothed_val, 1)
                smoothed_values.append(rounded_val)
            return smoothed_values
        
    except ValueError:
        pass
    
    return None

def main():
    """Main function to handle serial communication."""
    ser = None
    last_printed_f3_value = None 

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)
        print(f"Connected to {SERIAL_PORT}")
        print(f"Smoothing window size: {SMOOTHING_WINDOW_SIZE}")


        history_buffers = [collections.deque(maxlen=SMOOTHING_WINDOW_SIZE) for _ in range(NUM_VALUES)]

        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                
                smoothed_data = parse_and_process_line(line, history_buffers)
                
                if smoothed_data:
                    current_f3_value = smoothed_data[F3_INDEX]

                    # Check if the current f3 value is different from the previous one
                    if current_f3_value != last_printed_f3_value:
                        print(f"Smoothed values (rounded): {smoothed_data}")
                        # Update the tracking variable
                        last_printed_f3_value = current_f3_value

    except serial.SerialException as e:
        print(f"Serial port error: {e}")
    except KeyboardInterrupt:
        print("Program terminated by user")
    finally:
        if ser and ser.isOpen():
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    main()
