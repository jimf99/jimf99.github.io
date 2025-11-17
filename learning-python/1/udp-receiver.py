import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening for comma-separated floats on UDP port {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(1024) 
    
    # Decode the bytes to a string
    message_str = data.decode('utf-8')
    print(f"\nReceived raw message: {message_str} from {addr}")
    
    try:
        # Split the string by comma and convert each part to a float
        float_values = [float(x.strip()) for x in message_str.split(',')]
        
        if len(float_values) == 4:
            f1, f2, f3, f4 = float_values
            print(f"Parsed floats: f1={f1}, f2={f2}, f3={f3}, f4={f4}")
        else:
            print(f"Error: Expected 4 floats, but found {len(float_values)}")
            
    except ValueError as e:
        print(f"Error parsing floats: {e}")
