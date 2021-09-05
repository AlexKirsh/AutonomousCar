# Import Math libraries
import numpy as np
import matplotlib.pyplot as plt
import cv2


# Populate location coordinates in the room map matrix
def populate_map_points(ir_data, sonar_data, strt_pnt, distance, direction):
    # Get finish point coordinates
    fnsh_point_y = strt_pnt[0]
    if direction == 1:  # If robot moved forward
        fnsh_point_x = strt_pnt[1] + distance
    else:
        fnsh_point_x = strt_pnt[1] - distance
    fnsh_pnt = (fnsh_point_y, fnsh_point_x)

    data_points_num = np.linspace(strt_pnt[1], fnsh_pnt[1], len(ir_data), dtype=int)  # X coordinates

    cords = np.empty((0, 2), dtype=np.int32)
    if direction == 1:  # If robot moved forward
        # Fill IR data points to draw the outline of the walls
        for i in range(len(ir_data)):
            cords = np.append(cords, np.array([[data_points_num[i], strt_pnt[0] - ir_data[i]]]), axis=0)
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            cords = np.append(cords, np.array([[data_points_num[-1] + sonar_data, strt_pnt[0]-10 - i]]), axis=0)
    else:
        # Fill IR data points to draw the outline of the walls
        for i in range(len(ir_data)):
            cords = np.append(cords, np.array([[data_points_num[i], strt_pnt[0] + ir_data[i]]]), axis=0)
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            cords = np.append(cords, np.array([[data_points_num[-1] + sonar_data, strt_pnt[0]-10 - i]]), axis=0)

    return cords


mask = np.zeros((300,500), dtype=np.uint8) # Background

drive_direction = 1
ir_data = [34,36,36,33,33,38,35,22,10,10,10,10,10,10,10,11,10,14,21,25,25,22,23,25,22,23,25,25,24,22,24,25,23,24,24,22,22,25,24,25,25,23,22,23,27,25,12,12,11,11,13,13,12]
strt_loc = (250, 15)

room_coordinates = populate_map_points(ir_data, 56, strt_loc, 310, drive_direction)
ctr = np.array(room_coordinates).reshape((-1,1,2)).astype(np.int32)
cv2.drawContours(mask,[ctr],0,(255,255,255),1)
cv2.imshow('output', mask)
cv2.waitKey(0)