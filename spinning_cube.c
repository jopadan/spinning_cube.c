#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 100
#define HEIGHT 50

typedef struct {
  float x;
  float y;
  float z;
} Point3D;

typedef struct {
  int x;
  int y;
} Point2D;

/*
= rotate in x axis
   [ 1      0        0    ]
   | 0   cos(a)   -sin(a) |
   [ 0   sin(a)    cos(a) ]
*/
void rotate_x(Point3D* point, float angle) {
  // float x = point->x;
  float y = point->y * cos(angle) + point->z * sin(angle);
  float z = -(point->y * sin(angle)) + point->z * cos(angle);
  // point->x = x;
  point->y = y;
  point->z = z;
}

/*
= rotate in y axis
   [ cos(a)    0    sin(a) ]
   |   0       1      0    |
   [ -sin(a)   0    cos(a) ]
*/
void rotate_y(Point3D* point, float angle) {
  float x = point->x * cos(angle) - point->z * sin(angle);
  // float y = point->y;
  float z = point->x * sin(angle) + point->z * cos(angle);
  point->x = x;
  // point->y = y;
  point->z = z;
}

/*
= rotate in z axis
   [ cos(a)   -sin(a)   0  ]
   | sin(a)    cos(a)   0  |
   [   0         0      1  ]
*/
void rotate_z(Point3D* point, float angle) {
  float x = point->x * cos(angle) + point->y * sin(angle);
  float y = -(point->x * sin(angle)) + point->y * cos(angle);
  // float z = point->z;
  point->x = x;
  point->y = y;
  // point->z = z;
}

/*
= project 3d into 2d
    factor = z * scale
    x' = x + factor
    y' = y + factor
*/
Point2D project(Point3D* point, float scale) {
  int camera_distance = 4;
  float factor = (camera_distance / (camera_distance + point->z)) * scale;
  Point2D projected_point;
  projected_point.x = (int)(point->x * factor * WIDTH / 2 + WIDTH / 2);
  projected_point.y = (int)(point->y * factor * HEIGHT / 2 + HEIGHT / 2);
  return projected_point;
}

// passes points by reference so rotate functions modify the object
void rotate_cube(Point3D* points, float angle) {
  for (int i = 0; i < 8; i++) {
    rotate_x(&points[i], angle);
    rotate_y(&points[i], angle);
    rotate_z(&points[i], angle);
  }
}

// this is Bresenham's line algorithm
// dx/dy are the positive difference between the x/y coords of two points
// sx/sy are the unit step and direction of the line:
//   - positive (x: left to right, y: bottom to top)
//   - negative (x: right to left, y: top to bottom)
// err determines whether to move in the x-axis or y-axis based on dx,dy
// if err is positive than dx > dy, else the oposite is true
void draw_line(Point2D points[2], char c, char screen[HEIGHT][WIDTH]) {
  int x0 = points[0].x, x1 = points[1].x;
  int y0 = points[0].y, y1 = points[1].y;

  int dx = abs(x0 - x1), dy = abs(y0 - y1);
  int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, temp;

  while (1) {
    if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT) {
      screen[y0][x0] = c;
    }
    if (x0 == x1 && y0 == y1) {
      break;
    }

    temp = err;
    if (temp > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (temp < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

void draw_face(Point2D points[4], char c, char screen[HEIGHT][WIDTH]) {
  draw_line((Point2D[]){points[0], points[1]}, c, screen);
  draw_line((Point2D[]){points[1], points[2]}, c, screen);
  draw_line((Point2D[]){points[2], points[3]}, c, screen);
  draw_line((Point2D[]){points[3], points[0]}, c, screen);
}

void draw_cube(Point3D* points, float scale) {
  char screen[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      screen[i][j] = ' ';
    }
  }

  Point2D projected_points[8];
  for (int i = 0; i < 8; i++) {
    projected_points[i] = project(&points[i], scale);
  }

  draw_face((Point2D[]){projected_points[0], projected_points[1],
                        projected_points[2], projected_points[3]},
            '#', screen);

  draw_face((Point2D[]){projected_points[4], projected_points[5],
                        projected_points[6], projected_points[7]},
            '@', screen);
  draw_face((Point2D[]){projected_points[0], projected_points[1],
                        projected_points[5], projected_points[4]},
            '-', screen);
  draw_face((Point2D[]){projected_points[2], projected_points[3],
                        projected_points[7], projected_points[6]},
            '=', screen);
  draw_face((Point2D[]){projected_points[0], projected_points[3],
                        projected_points[7], projected_points[4]},
            'L', screen);
  draw_face((Point2D[]){projected_points[1], projected_points[2],
                        projected_points[6], projected_points[5]},
            '*', screen);

  printf("\x1b[H");  // moves cursor to home position
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      putchar(screen[i][j]);
    }
    putchar('\n');
  }
}

int main() {
  Point3D cube[8] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                     {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

  float angle = 0.03;
  float scale = 0.4;

  while (1) {
    rotate_cube(cube, angle);
    draw_cube(cube, scale);
    usleep(30000);  // sleep for 30 ms
  }
}