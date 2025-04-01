import socket
import struct
import matplotlib.pyplot as plt

HOST = "127.0.0.1"  # Localhost
PORT = 12345        # Port to connect to

class Ship:
    def __init__(self, name, position, speed, size):
        self.name = name
        self.position = position
        self.speed = speed
        self.size = size

    def __repr__(self):
        return f"Ship(Name={self.name}, Position={self.position}, Speed={self.speed}, Size={self.size})"

def receive_ship(client_socket):
    # Receive the serialized data
    buffer = client_socket.recv(1024)

    # Deserialize the data
    name = buffer[:50].decode('utf-8').strip('\x00')  # Decode the name (50 bytes)
    position = struct.unpack("dd", buffer[50:66])     # Unpack two doubles for position
    speed = struct.unpack("d", buffer[66:74])[0]      # Unpack one double for speed
    size = struct.unpack("d", buffer[74:82])[0]       # Unpack one double for size

    return Ship(name, position, speed, size)

def plot_ships(ships):
    # Create a new figure
    plt.figure(figsize=(10, 6))

    # Group ships by name to plot their paths
    ship_paths = {}
    for ship in ships:
        if ship.name not in ship_paths:
            ship_paths[ship.name] = {"x": [], "y": []}
        ship_paths[ship.name]["x"].append(ship.position[0])
        ship_paths[ship.name]["y"].append(ship.position[1])

    # Plot the paths of the ships
    for name, path in ship_paths.items():
        plt.plot(
            path["x"], path["y"], linestyle=":", marker="o", label=name
        )  # Dotted line with markers

    # Annotate each ship's final position with its name, speed, and size
    for ship in ships:
        plt.text(
            ship.position[0],
            ship.position[1],
            f"{ship.name}\nSpeed: {ship.speed}\nSize: {ship.size}",
            fontsize=8,
            ha="center",
            va="bottom",
        )

    # Customize the axes
    plt.xlabel("X Position")
    plt.ylabel("Y Position")
    plt.title("Ship Movement Paths")
    plt.grid(True)
    plt.legend()

    # Show the plot
    plt.show()

if __name__ == "__main__":
    # Connect to the server
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        print("Connected to the server. Waiting for data...")

        # Receive the number of ships
        num_ships = struct.unpack("i", client_socket.recv(4))[0]
        print(f"Receiving {num_ships} ships...")

        # Receive each ship
        ships = []
        for _ in range(num_ships):
            ship = receive_ship(client_socket)
            ships.append(ship)

        # Print the received ships
        print("Received ships:")
        for ship in ships:
            print(ship)

        # Plot the received ships
        plot_ships(ships)

    print("All data received. Exiting.")