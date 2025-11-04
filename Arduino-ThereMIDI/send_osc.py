from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import udp_client
import threading
import time
from datetime import datetime
import random

def print_osc_message(address: str, fnc: float, leftDistance: float, rightDistance: float, freq: float):
    """Callback to handle incoming OSC messages with a timestamp.

    Args:
        address (str): The OSC address of the received message.
        fnc (float): First argument of the message.
        leftDistance (float): Second argument of the message.
        rightDistance (float): Third argument of the message.
        freq (float): Fourth argument of the message.
    """
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] <<< {address} [fnc: {fnc}, leftDistance: {leftDistance}, rightDistance: {rightDistance}, freq: {freq}]")

def start_receiver(ip: str, port: int):
    """Setup and start the OSC receiver.

    Args:
        ip (str): The IP address to listen for OSC messages.
        port (int): The port to listen for OSC messages.
    """
    osc_dispatcher = dispatcher.Dispatcher()
    osc_dispatcher.map("/example/address", print_osc_message)  # Map the OSC address to the handler
    
    osc_server_instance = osc_server.BlockingOSCUDPServer((ip, port), osc_dispatcher)
    print(f"Listening on {ip}:{port}...")
    
    # Serve forever and handle incoming messages
    osc_server_instance.serve_forever()

def send_osc_message(ip_address: str, port: int, address: str, fnc: float, leftDistance: float, rightDistance: float, freq: float):
    """Send OSC message with four arguments and a timestamp.

    Args:
        ip_address (str): The IP address of the OSC recipient.
        port (int): The port of the OSC recipient.
        address (str): The OSC address for the message.
        fnc (float): First argument of the message.
        leftDistance (float): Second argument of the message.
        rightDistance (float): Third argument of the message.
        freq (float): Fourth argument of the message.
    """
    client = udp_client.SimpleUDPClient(ip_address, port)
    
    # Send the OSC message with four arguments
    client.send_message(address, (fnc, leftDistance, rightDistance, freq))
    timestamp = datetime.now().strftime("%H:%M:%S")
    # print(f"[{timestamp}] >>> {address} [fnc: {fnc}, leftDistance: {leftDistance}, rightDistance: {rightDistance}, freq: {freq}]")

def truncate(value, decimal_places):
    factor = 10 ** decimal_places
    return int(value * factor) / factor

def start_sender(receiver_ip: str, receiver_port: int):
    """Send OSC messages at a specified frequency (20 Hz).

    Args:
        receiver_ip (str): The IP address of the OSC receiver.
        receiver_port (int): The port of the OSC receiver.
    """
    osc_address = "/example/address"  # OSC address for the messages
    
    counter = 0  # Initialize a counter for generating argument values
    while True:
        # Prepare arguments for the OSC message
        fnc = 0
        leftDistance = int(random.uniform(0, 200))
        rightDistance = int(random.uniform(0, 200))
        freq=int((rightDistance/200)*2000)+55
        
        # Send the OSC message
        send_osc_message(receiver_ip, receiver_port, osc_address, fnc, leftDistance, rightDistance, freq)
        
        # Increment the counter for the next message
        counter += 1
        
        # Send messages every 
        time.sleep(0.1)

if __name__ == "__main__":
    # Configure the parameters for the OSC receiver and sender
    receiver_ip = "127.0.0.1"  # IP address to listen for messages
    receiver_port = 4560        # Port to listen for messages

    # Start the OSC sender in a separate thread
    sender_thread = threading.Thread(target=start_sender, args=(receiver_ip, receiver_port))
    sender_thread.daemon = True  # Allow the thread to exit when the main program does
    sender_thread.start()  # Start the sender thread

    # Allow the sender thread to start
    time.sleep(1)

    # Start the OSC receiver in the main thread
    start_receiver(receiver_ip, receiver_port)
