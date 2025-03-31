# To run this script, use the command:
# python3 test.py
# While in the directory containing this script

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Define waypoints (latitude, longitude, altitude)
A = (0, 0, 0)        # Takeoff
B = (500, 300, 10)   # Climbing to 10km
C = (5500, 3500, 10) # Cruising at 10km
D = (6000, 4000, 0)  # Landing

# Number of points for smooth path
num_points_climb = 50
num_points_cruise = 200
num_points_descend = 50

# Generate path sections
x_climb = np.linspace(A[0], B[0], num_points_climb)
y_climb = np.linspace(A[1], B[1], num_points_climb)
z_climb = np.linspace(A[2], B[2], num_points_climb)

x_cruise = np.linspace(B[0], C[0], num_points_cruise)
y_cruise = np.linspace(B[1], C[1], num_points_cruise)
z_cruise = np.full(num_points_cruise, 10)  # Constant altitude

x_descend = np.linspace(C[0], D[0], num_points_descend)
y_descend = np.linspace(C[1], D[1], num_points_descend)
z_descend = np.linspace(C[2], D[2], num_points_descend)

# Combine all sections
x_values = np.concatenate((x_climb, x_cruise, x_descend))
y_values = np.concatenate((y_climb, y_cruise, y_descend))
z_values = np.concatenate((z_climb, z_cruise, z_descend))

# Plot 3D flight path
fig = plt.figure(figsize=(10, 6))
ax = fig.add_subplot(111, projection='3d')

# Dashed flight path (use linestyle='--' for dashed, ':' for dotted)
ax.plot(x_values, y_values, z_values, linestyle="--", linewidth=2, color="b", label="Flight Path")

# Mark key points
ax.scatter(*A, color='green', s=50, label="Takeoff (A)")
ax.scatter(*B, color='orange', s=50, label="Reached 10km (B)")
ax.scatter(*C, color='yellow', s=50, label="Cruising (C)")
ax.scatter(*D, color='red', s=50, label="Landing (D)")

# Annotate points
ax.text(*A, " A (Takeoff)", fontsize=10, color='green')
ax.text(*B, " B (10km Alt)", fontsize=10, color='orange')
ax.text(*C, " C (Cruising)", fontsize=10, color='yellow')
ax.text(*D, " D (Landing)", fontsize=10, color='red')

# Customize labels
ax.set_xlabel("Longitude (km)")
ax.set_ylabel("Latitude (km)")
ax.set_zlabel("Altitude (km)")
ax.set_title("3D Flight Path with Dashed Line")

# Show legend and grid
ax.legend()
ax.grid(True)

# Show the plot
plt.show()
