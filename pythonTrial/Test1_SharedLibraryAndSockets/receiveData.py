import socket

def receive_data():
    # Connect to the server
    host = "127.0.0.1"  # Localhost
    port = 12345

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((host, port))
        data = client_socket.recv(4)  # Receive 4 bytes (size of an int)
        value = int.from_bytes(data, byteorder="little")
        return value

if __name__ == "__main__":
    previous_value = -1
    while True:
        try:
            current_value = receive_data()
            if current_value != previous_value:
                print(f"Received new value: {current_value}")
                previous_value = current_value
        except ConnectionRefusedError:
            print("Server not available. Retrying...")