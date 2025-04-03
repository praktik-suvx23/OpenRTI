import socket
import struct
from collections import defaultdict

HOST = "127.0.0.1"  # Localhost
PORT = 12345        # Port to connect to

def receive_multimap(client_socket):
    # Receive the size of the multimap
    map_size = struct.unpack("i", client_socket.recv(4))[0]
    print(f"Receiving multimap with {map_size} entries...")

    # Initialize a defaultdict to store the multimap entries
    multimap = defaultdict(list)

    # Receive each entry in the multimap
    for _ in range(map_size):
        # Receive the pair (shooter index and target index)
        shooter_index = struct.unpack("i", client_socket.recv(4))[0]
        target_index = struct.unpack("i", client_socket.recv(4))[0]

        # Receive the size of the vector
        vector_size = struct.unpack("i", client_socket.recv(4))[0]

        # Receive the vector of doubles
        vector_data = struct.unpack(f"{vector_size}d", client_socket.recv(8 * vector_size))

        # Append the entry to the multimap
        multimap[(shooter_index, target_index)].append(list(vector_data))

    return multimap

if __name__ == "__main__":
    # Connect to the server
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        print("Connected to the server. Waiting for data...")

        # Receive the multimap
        multimap = receive_multimap(client_socket)

        # Print the received multimap
        print("Received multimap:")
        for pair, ranges_list in multimap.items():
            print(f"Shooter Index: {pair[0]}, Target Index: {pair[1]}")
            for ranges in ranges_list:
                print(f"  Ranges: {ranges}")

        # Perform calculations on the received multimap
        print("\nPerforming calculations on the received data:")
        for pair, ranges_list in multimap.items():
            # Example: Calculate the average range for each shooter-target pair
            for ranges in ranges_list:
                average_range = sum(ranges) / len(ranges)
                print(f"Average range for Shooter {pair[0]} -> Target {pair[1]}: {average_range:.2f}")

            # Example: Find the maximum range for each shooter-target pair
            max_range = max(max(ranges) for ranges in ranges_list)
            print(f"Maximum range for Shooter {pair[0]} -> Target {pair[1]}: {max_range:.2f}")

    print("All data received and calculations completed. Exiting.")