import socket
import threading
import sys
import time
import os

HOST = '127.0.0.1'
PORT = 65432

# --- Server Code ---
class Server:
    def __init__(self):
        self.clients = {} # Dictionary mapping conn to {'addr': addr, 'nick': nickname}
        self.lock = threading.Lock()
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind((HOST, PORT))
        self.server_socket.listen()
        print(f"Server listening on {HOST}:{PORT}")
        print("You can now type messages to clients directly in this console.")

    def handle_client(self, conn, addr):
        # Client is expected to send their desired nickname first thing.
        try:
            nick_data = conn.recv(1024)
            if not nick_data:
                return self.remove_client(conn)
            nickname = nick_data.decode('utf-8').strip()

            if not nickname:
                nickname = f"User-{addr[1]}" # Fallback nick
            
            with self.lock:
                self.clients[conn] = {'addr': addr, 'nick': nickname}
            
            print(f"[NEW CONNECTION] {nickname} ({addr}) connected.")
            self.broadcast(f"[SERVER] {nickname} has joined the chat.\n")

            while True:
                data = conn.recv(1024)
                if not data:
                    break
                message = data.decode('utf-8').strip()

                if message.lower() == 'quit':
                    break
                else:
                    current_nick = self.clients[conn]['nick']
                    print(f"[{current_nick}] {message}")
                    self.broadcast(f"[{current_nick}] {message}\n", conn)
        except Exception as e:
            # print(f"[ERROR] Connection with {addr} failed: {e}")
            pass # Error handling can be cleaner in a simple example
        finally:
            self.remove_client(conn)

    def broadcast(self, message, sender_conn=None):
        with self.lock:
            for client_socket in self.clients:
                if sender_conn is None or client_socket != sender_conn:
                    try:
                        client_socket.sendall(message.encode('utf-8'))
                    except:
                        pass # Connection likely dead, will be cleaned up by handle_client

    def server_input_handler(self):
        while True:
            try:
                message = sys.stdin.readline()
                if message.strip().lower() == 'quit':
                    break
                full_message = f"[SERVER] {message}"
                self.broadcast(full_message)
            except KeyboardInterrupt:
                break
            except EOFError:
                break
        self.stop_server()

    def remove_client(self, conn):
        with self.lock:
            if conn in self.clients:
                info = self.clients.pop(conn)
                nick = info['nick']
                addr = info['addr']
                print(f"[DISCONNECTION] {nick} ({addr}) disconnected.")
                self.broadcast(f"[SERVER] {nick} has left the chat.\n")
                conn.close()

    def start(self):
        input_thread = threading.Thread(target=self.server_input_handler, daemon=True)
        input_thread.start()
        
        try:
            while True:
                conn, addr = self.server_socket.accept()
                thread = threading.Thread(target=self.handle_client, args=(conn, addr))
                thread.start()
        except KeyboardInterrupt:
            pass
        finally:
            self.stop_server()

    def stop_server(self):
        print("[SERVER] Shutting down.")
        # Close all client sockets before closing server socket
        with self.lock:
            for client in list(self.clients.keys()):
                client.close()
        self.server_socket.close()
        os._exit(0) # Force exit all threads cleanly

# --- Client Code ---
class Client:
    def __init__(self, nickname):
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.nickname = nickname
        
    def receive_messages(self):
        while True:
            try:
                data = self.client_socket.recv(1024)
                if not data:
                    print("\n[CLIENT] Disconnected from server.")
                    break
                # Clear the current input prompt line before printing message
                sys.stdout.write(f"\r{data.decode('utf-8').strip()}\n")
                sys.stdout.write("You: ")
                sys.stdout.flush()
            except:
                break

    def send_messages(self):
        sys.stdout.write("You: ")
        sys.stdout.flush()
        while True:
            try:
                # Read input without readline to better handle prompt clearing
                message = sys.stdin.readline().strip()
                if not message:
                    sys.stdout.write("You: ")
                    sys.stdout.flush()
                    continue
                    
                self.client_socket.sendall(message.encode('utf-8'))

                if message.lower() == 'quit':
                    break
                
                sys.stdout.write("You: ")
                sys.stdout.flush()
            except EOFError:
                break
            except KeyboardInterrupt:
                break

    def start(self):
        try:
            self.client_socket.connect((HOST, PORT))
            # Send nickname immediately upon connection
            self.client_socket.sendall(self.nickname.encode('utf-8')) 
            
            print(f"[CLIENT] Connected to {HOST}:{PORT} as '{self.nickname}'. Type 'quit' to exit.")
            
            receive_thread = threading.Thread(target=self.receive_messages)
            send_thread = threading.Thread(target=self.send_messages)
            
            receive_thread.start()
            send_thread.start()

            send_thread.join()
        except ConnectionRefusedError:
            print(f"[CLIENT] Connection refused. Is the server running on {HOST}:{PORT}?")
        except KeyboardInterrupt:
            pass
        finally:
            self.client_socket.close()

# --- Main execution ---
def main_menu():
    while True:
        print("\n--- Chat App Menu ---")
        print("1. Start Server")
        print("2. Start Client")
        print("3. Exit")
        choice = input("Enter your choice (1, 2, or 3): ").strip()

        if choice == "1":
            server = Server()
            server.start()
            break # Program flow generally stays in server.start() until shutdown
        elif choice == "2":
            nickname = input("Enter your desired nickname: ").strip()
            if not nickname:
                print("Nickname cannot be empty.")
                continue
            client = Client(nickname)
            client.start()
            break # Exit loop after client finishes
        elif choice == "3":
            print("Exiting application.")
            sys.exit(0)
        else:
            print("Invalid choice. Please enter 1, 2, or 3.")

if __name__ == "__main__":
    main_menu()
