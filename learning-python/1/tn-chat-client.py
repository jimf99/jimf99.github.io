# This is a conceptual example using telnetlib for a Python client.
# For a basic chat, using your OS's telnet client is often simpler.

import telnetlib
import threading
import sys

HOST = '127.0.0.1'
PORT = 50000

def receive_messages(tn):
    """Continuously receives and prints messages from the server."""
    while True:
        try:
            message = tn.read_until(b"\n")
            sys.stdout.write(message.decode('utf-8'))
            sys.stdout.flush()
        except EOFError:
            print("Server disconnected.")
            break

def start_client():
    """Starts the Telnet chat client."""
    try:
        tn = telnetlib.Telnet(HOST, PORT)
    except ConnectionRefusedError:
        print(f"Could not connect to server at {HOST}:{PORT}. Is the server running?")
        return

    print("Connected to Telnet Chat Server. Type your messages and press Enter.")

    # Start a thread to receive messages
    receive_thread = threading.Thread(target=receive_messages, args=(tn,))
    receive_thread.daemon = True  # Allow main program to exit even if this thread is running
    receive_thread.start()

    # Send messages from user input
    try:
        while True:
            message = sys.stdin.readline()
            tn.write(message.encode('utf-8'))
    except KeyboardInterrupt:
        print("\nDisconnecting...")
    finally:
        tn.close()

if __name__ == "__main__":
    start_client()