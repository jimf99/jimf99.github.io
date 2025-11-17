import socket
import threading

HOST = '127.0.0.1'  # Localhost
PORT = 50000        # Arbitrary non-privileged port
clients = []        # List to store connected client sockets

def broadcast(message, sender_socket):
    """Sends a message to all connected clients except the sender."""
    for client in clients:
        if client != sender_socket:
            try:
                client.send(message)
            except:
                # Client disconnected
                remove_client(client)

def handle_client(client_socket, address):
    """Handles communication with a single client."""
    print(f"New connection from {address}")
    client_socket.send(b"Welcome to the Telnet Chat Server!\n")
    clients.append(client_socket)

    try:
        while True:
            message = client_socket.recv(1024)
            if not message:
                break  # Client disconnected
            
            # Decode message for display and re-encode for broadcasting
            decoded_message = message.decode('utf-8').strip()
            print(f"Received from {address}: {decoded_message}")
            
            broadcast_message = f"<{address[0]}> {decoded_message}\n".encode('utf-8')
            broadcast(broadcast_message, client_socket)

    except ConnectionResetError:
        print(f"Client {address} disconnected unexpectedly.")
    finally:
        remove_client(client_socket)
        print(f"Connection from {address} closed.")

def remove_client(client_socket):
    """Removes a client from the list of connected clients."""
    if client_socket in clients:
        clients.remove(client_socket)
        client_socket.close()

def start_server():
    """Starts the Telnet chat server."""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(5)  # Max 5 pending connections
    print(f"Telnet Chat Server listening on {HOST}:{PORT}")

    while True:
        client_socket, address = server_socket.accept()
        client_handler = threading.Thread(target=handle_client, args=(client_socket, address))
        client_handler.start()

if __name__ == "__main__":
    start_server()