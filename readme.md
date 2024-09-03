# 3d to 2d in c and unix terminal

this is a demo on how to render 3d objects on a 2d screen with c and any unix terminal. the code simulates a rotating cube projected onto a terminal screen using basic linear algebra and computer graphics

### 1. rotation matrices
rotation matrices are used to rotate the cube in 3d space around the x, y, and z axes. the specific rotation matrices applied are:

- **rotation around x-axis:**

- **rotation around y-axis:**

- **rotation around z-axis:**


### 2. projection from 3d to 2d
the projection method used to map 3d points to a 2d plane is based on a simple perspective projection. given a point *(x, y, z)* in 3d space, its projection *(x', y')* on a 2d plane is calculated as:


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