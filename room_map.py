# Import Math libraries
import numpy as np
import matplotlib.pyplot as plt


# Populate location points in the room map matrix
def populate_map_points(map_mat, ir_data, sonar_data, strt_pnt, distance, direction):
    # Get finish point coordinates
    fnsh_point_y = strt_pnt[0]
    if direction == 1:  # If robot moved forward
        fnsh_point_x = strt_pnt[1] + distance
    else:
        fnsh_point_x = strt_pnt[1] - distance
    fnsh_pnt = (fnsh_point_y, fnsh_point_x)

    data_points_num = np.linspace(strt_pnt[1], fnsh_pnt[1], len(ir_data), dtype=int)  # X coordinates

    if direction == 1:  # If robot moved forward
        # Fill IR data points to draw the outline of the walls
        for i in range(len(ir_data)):
            map_mat[strt_pnt[0] + ir_data[i], data_points_num[i]] = 1
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            map_mat[strt_pnt[0]-10 + i, data_points_num[-1] + sonar_data] = 1
    else:
        # Fill IR data points to draw the outline of the walls
        for i in range(len(ir_data)):
            map_mat[strt_pnt[0] - ir_data[i], data_points_num[i]] = 1
        # Mark the obstacle as 20 dots in front of the robot
        for i in range(20):
            map_mat[strt_pnt[0]-10 + i, data_points_num[-1] + sonar_data] = 1

    return map_mat

'''
room_map_mat = np.zeros((300, 500))
drive_direction = 1
ir_data = [34,36,36,33,33,38,35,22,10,10,10,10,10,10,10,11,10,14,21,25,25,22,23,25,22,23,25,25,24,22,24,25,23,24,24,22,22,25,24,25,25,23,22,23,27,25,12,12,11,11,13,13,12]
strt_loc = (250, 15)
room_map_mat = populate_map_points(room_map_mat, ir_data, 56, strt_loc, 310, drive_direction)

plt.imshow(room_map_mat, cmap = 'Greys')
plt.show()
'''