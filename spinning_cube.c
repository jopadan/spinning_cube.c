#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

unsigned short int WIDTH  = 200;
unsigned short int HEIGHT = 100;
float              ASPECT = (float)200/(float)100;

typedef float __attribute__((vector_size(4*sizeof(float)))) Point3D;
typedef int   __attribute__((vector_size(2*sizeof(int  )))) Point2D;  

/*
= rotate in x axis
   [ 1      0        0    ]
   | 0   cosf(a)   -sinf(a) |
   [ 0   sinf(a)    cosf(a) ]
*/
void rotate_x(Point3D* point, float angle) {
  // float x = point->x;
  float y = (*point)[1] * cosf(angle) + (*point)[2] * sinf(angle);
  float z = -((*point)[1] * sinf(angle)) + (*point)[2] * cosf(angle);
  // point->x = x;
  (*point)[1] = y;
  (*point)[2] = z;
}

/*
= rotate in y axis
   [ cosf(a)    0    sinf(a) ]
   |   0       1      0    |
   [ -sinf(a)   0    cosf(a) ]
*/
void rotate_y(Point3D* point, float angle) {
  float x = (*point)[0] * cosf(angle) - (*point)[2] * sinf(angle);
  // float y = point->y;
  float z = (*point)[0] * sinf(angle) + (*point)[2] * cosf(angle);
  (*point)[0] = x;
  // point->y = y;
  (*point)[2] = z;
}

/*
= rotate in z axis
   [ cosf(a)   -sinf(a)   0  ]
   | sinf(a)    cosf(a)   0  |
   [   0         0      1  ]
*/
void rotate_z(Point3D* point, float angle) {
  float x = (*point)[0] * cosf(angle) + (*point)[1] * sinf(angle);
  float y = -((*point)[0] * sinf(angle)) + (*point)[1] * cosf(angle);
  // float z = point->z;
  (*point)[0] = x;
  (*point)[1] = y;
  // point->z = z;
}

/*
= project 3d into 2d
    factor = z * scale
    x' = x + factor
    y' = y + factor
*/
Point2D project(Point3D point, float scale) {
  int camera_distance = 4;
  float factor = (camera_distance / (camera_distance + point[2])) * scale;
  Point2D projected_point;
  projected_point[0] = (int)(point[0] * factor * WIDTH  / ASPECT + WIDTH  / 2 );
  projected_point[1] = (int)(point[1] * factor * HEIGHT / 2      + HEIGHT / 2 );
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
void draw_line(Point2D a, Point2D b, char c, char screen[HEIGHT][WIDTH]) {
  Point2D x = { a[0], b[0] };
  Point2D y = { a[1], b[1] };
  Point2D d = { fabsf(x[0] - x[1]), fabsf(y[0] - y[1]) };
  Point2D s = { x[0] < x[1] ? 1 : -1, y[0] < y[1] ? 1 : -1 };

  int err = (d[0] > d[1] ? d[0] : -d[1]) / 2, temp;

  while (1) {
    if (x[0] >= 0 && x[0] < WIDTH && y[0] >= 0 && y[0] < HEIGHT) {
      screen[y[0]][x[0]] = c;
    }
    if (x[0] == x[1] && y[0] == y[1]) {
      break;
    }

    temp = err;
    if (temp > -d[0]) {
      err -= d[1];
      x[0] += s[0];
    }
    if (temp < d[1]) {
      err += d[0];
      y[0] += s[1];
    }
  }
}

void draw_face(Point2D* p, size_t i0, size_t i1, size_t i2, size_t i3, char c, char screen[HEIGHT][WIDTH]) {
  draw_line(p[i0], p[i1], c, screen);
  draw_line(p[i1], p[i2], c, screen);
  draw_line(p[i2], p[i3], c, screen);
  draw_line(p[i3], p[i0], c, screen);
}

void draw_cube(Point3D points[8], float scale) {
  char screen[HEIGHT][WIDTH];
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      screen[i][j] = ' ';
    }
  }

  Point2D projected_points[8];
  for (int i = 0; i < 8; i++)
    projected_points[i] = project(points[i], scale);

  draw_face(projected_points, 0,1,2,3, '#', screen);
  draw_face(projected_points, 4,5,6,7, '@', screen);
  draw_face(projected_points, 0,1,5,4, '-', screen);
  draw_face(projected_points, 2,3,7,6, '=', screen);
  draw_face(projected_points, 0,3,7,4, 'L', screen);
  draw_face(projected_points, 1,2,6,5, '*', screen);

  printf("\x1b[H");  // moves cursor to home position
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      putchar(screen[i][j]);
    }
    putchar('\n');
  }
}

static inline void sys_cleanup_cb();
static inline void sys_signal_cb(int sig);
static inline bool sys_update();

int main() {
  Point3D cube[8] = { {-1, -1, -1, 0}, {1, -1, -1, 0}, {1, 1, -1, 0}, {-1, 1,-1, 0},
                      {-1, -1, 1,  0}, {1, -1,  1, 0}, {1, 1,  1, 0}, {-1, 1, 1, 0} };

  float angle = 0.03;
  float scale = 0.4;

  atexit(sys_cleanup_cb);

  while (sys_update())
  {
    rotate_cube(cube, angle);
    draw_cube(cube, scale);
  }

  exit(EXIT_SUCCESS);
}

static inline void sys_cleanup_cb()
{
	struct termios info;
	tcgetattr(0, &info);
	info.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &info);
}

static inline void sys_signal_cb(int sig)
{
	switch(sig)
	{
		case SIGINT:
			exit(EXIT_SUCCESS);
			break;
		case SIGWINCH:
			{
				struct winsize w;
				ioctl(0, TIOCGWINSZ, &w);
				WIDTH  = w.ws_col;
				HEIGHT = w.ws_row;
				ASPECT = (float)WIDTH/(float)HEIGHT;
			}
			break;
		default:
			printf("Unhandled signal: %s\n", strsignal(sig));
			break;
	}
}

static inline bool sys_update()
{
  	static bool first = true;
	if(first)
	{
		first = false;
		struct termios info;
		tcgetattr(0,&info);
		info.c_lflag &= ~ICANON;
		info.c_cc[VMIN]  = 1;
		info.c_cc[VTIME] = 0;
		tcsetattr(0,TCSANOW, &info);
		fcntl(0, F_SETFL, O_NONBLOCK);

		struct sigaction action;
		action.sa_flags = 0;
		action.sa_handler = sys_signal_cb;
		sigemptyset(&action.sa_mask);
		sigaction(SIGINT  , &action, 0);
		sigaction(SIGWINCH, &action, 0);
		sys_signal_cb(SIGWINCH);
	}
	int ch = getchar();
    	usleep(30000);  // sleep for 30 ms
	return (ch != 27) && (ch != '\n') && (ch != 32);
}

