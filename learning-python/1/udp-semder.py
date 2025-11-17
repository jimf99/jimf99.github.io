import socket
import time

UDP_IP = "127.0.0.1"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Example data: four floats
data_points = [1.25, 20.0, 350.123, 4000.0]

while True:
    # Format the floats into a comma-separated string
    MESSAGE = f"{data_points[0]}, {data_points[1]}, {data_points[2]}, {data_points[3]}"
    
    # Send the string encoded as bytes
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))
    print(f"Sent message: {MESSAGE}")
    
    # Update data for next send (optional)
    data_points[0] += 0.1
    data_points[1] += 0.5
    data_points[2] += 1.0
    data_points[3] += 10.0

    time.sleep(1)
