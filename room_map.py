# Import Math libraries
import numpy as np
import matplotlib.pyplot as plt
import cv2


# Populate location coordinates in the room map matrix
def populate_map_points(ir_data, sonar_data, strt_pnt, distance, direction):
    # Get finish point coordinates
    fnsh_point_y = strt_pnt[1]
    if direction == 1:  # If robot moved forward
        fnsh_point_x = strt_pnt[0] + distance
    else:
        fnsh_point_x = strt_pnt[0] - distance
    fnsh_pnt = (fnsh_point_x, fnsh_point_y)

    x_coordinates = np.linspace(strt_pnt[0], fnsh_pnt[0], len(ir_data), dtype=int)  # X coordinates

    if direction == 1:  # If robot moved forward
        # Fill IR data points to draw the outline of the walls
        cords = np.array(list(zip(x_coordinates, strt_pnt[1] - ir_data)))
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            cords = np.append(cords, np.array([[x_coordinates[-1] + sonar_data, strt_pnt[1]-10 - i]]), axis=0)
    else:
        # Fill IR data points to draw the outline of the walls
        cords = np.array(list(zip(x_coordinates, strt_pnt[1] + ir_data)))
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            cords = np.append(cords, np.array([[x_coordinates[-1] + sonar_data, strt_pnt[1]-10 - i]]), axis=0)

    return cords


mask = np.zeros((300,500), dtype=np.uint8) # Background

drive_direction = 1
ir_data = np.array([34,36,36,33,33,38,35,22,10,10,10,10,10,10,10,11,10,14,21,25,25,22,23,25,22,23,25,25,24,22,24,25,23,24,24,22,22,25,24,25,25,23,22,23,27,25,12,12,11,11,13,13,12])
strt_loc = (15, 250)

room_coordinates = populate_map_points(ir_data, 56, strt_loc, 310, drive_direction)
ctr = np.array(room_coordinates).reshape((-1,1,2)).astype(np.int32)
cv2.drawContours(mask,[ctr],0,(255,255,255),1)
cv2.imshow('output', mask)
cv2.waitKey(0)