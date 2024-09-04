# 3d to 2d in c and unix terminal

this is a demo on how to render 3d objects on a 2d screen with c and any unix terminal. the code simulates a rotating cube projected onto a terminal screen using basic linear algebra and computer graphics

![cube](https://github.com/user-attachments/assets/f909a6b9-4df4-4fff-a87b-22564f5789c5)

### 1. rotation matrices
rotation matrices are used to rotate the cube in 3d space around the x, y, and z axes. the specific rotation matrices applied are:

- **rotation around x-axis:**

![x (2)](https://github.com/user-attachments/assets/53bbe53d-51ce-4f89-bd60-1e1b45437969)

- **rotation around y-axis:**

![y (2)](https://github.com/user-attachments/assets/2e05516c-05bc-4ee7-9f8c-5757516cf087)

- **rotation around z-axis:**

![z (2)](https://github.com/user-attachments/assets/8729309a-5a52-48e3-b53a-00a8671c7efd)

### 2. projection from 3d to 2d
the projection method used to map 3d points to a 2d plane is based on a simple perspective projection. given a point *(x, y, z)* in 3d space, its projection *(x', y')* on a 2d plane is calculated as:

![p (2)](https://github.com/user-attachments/assets/3799fbf9-c2fe-4ab3-9219-07840660390b)

### 3. bresenham's line algorithm
the edges of the cube are drawn using bresenham's line algorithm. it calculates the points required to draw a line between two coordinates on a 2d grid, at terminal pixel density is really small, it runs fast and it's O(n)

## usage
compile the code with the following command:
```
make
```
run the compiled program:
```
./spinning_cube
```
