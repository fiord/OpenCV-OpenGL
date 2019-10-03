#define _USE_MATH_DEFINES
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <GL/glut.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test5"
#define TEXTURE_HEIGHT (1080)
#define TEXTURE_WIDTH (1080)

void init_GL(int, char**);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char, int, int);
void glut_mouse(int, int, int, int);
void glut_motion(int, int);
void glut_idle();
void draw_pyramid();

// global constants
double g_angle1 = 0.0;
double g_angle2 = -M_PI / 6.0;
double g_distance = 10.0;
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
GLuint g_TextureHandles[1] = {0};
int textureIndex = 0;

cv::VideoCapture cap;
cv::Mat frame;

int main(int argc, char *argv[]) {
  // init opengl
  init_GL(argc, argv);

  // init
  init();

  // register callback function
  set_callback_functions();

  // main loop
  glutMainLoop();

  return 0;
}

void init_GL(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}

void init() {
  // videocapture
  cap.open(0);
  if (!cap.isOpened()) {
    printf("cannnot open the camera\n");
    exit(0);
  }
  cv::namedWindow("camera", 1);
  glut_idle();

  glClearColor(0.2, 0.2, 0.2, 0.2);
  glGenTextures(1, g_TextureHandles);

  glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void set_callback_functions() {
  glutDisplayFunc(glut_display);
  glutKeyboardFunc(glut_keyboard);
  glutMouseFunc(glut_mouse);
  glutMotionFunc(glut_motion);
  glutPassiveMotionFunc(glut_motion);
  glutIdleFunc(glut_idle);
}

void glut_keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 'Q':
    case '\033':
      exit(0);
    default:
      break;
  }
  glutPostRedisplay();
}

void glut_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_UP) {
      g_isLeftButtonOn = false;
    }
    else if (button == GLUT_DOWN) {
      g_isLeftButtonOn = true;
    }
  }
  if (button == GLUT_RIGHT_BUTTON) {
    if (state =- GLUT_UP) {
      g_isRightButtonOn = false;
    }
    else if (button == GLUT_DOWN) {
      g_isRightButtonOn = true;
    }
  }
}

void glut_motion(int x, int y) {
  static int px = -1, py = -1;
  if (g_isLeftButtonOn) {
    if (px >= 0 && py >= 0) {
      g_angle1 += (double)-(x - px) / 20.0;
      g_angle2 += (double)(y - py) / 20.0;
    }
    px = x;
    py = y;
  }
  else if (g_isRightButtonOn) {
    if (px >= 0 && py >= 0) {
      g_distance += (double)(y - py) / 20.0;
    }
    px = x;
    py = y;
  }
  else {
    px = py = -1;
  }
  glutPostRedisplay();
}

void glut_display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, 1.0, 0.1, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (cos(g_angle2) > 0) {
    gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
        g_distance * sin(g_angle2),
        g_distance * cos(g_angle2) * cos(g_angle1),
        0, 0, 0, 0, -1, 0);
  }
  else {
    gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
        g_distance * sin(g_angle2),
        g_distance * cos(g_angle2) * cos(g_angle1),
        0, 0, 0, 0, 1, 0);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  draw_pyramid();
  glFlush();
  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

void glut_idle() {
  cap >> frame;
  cv::imshow("camera", frame);
  // BGR -> RGB
  cv::cvtColor(frame, frame, CV_BGR2RGB);

  glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
  glTexSubImage2D(GL_TEXTURE_2D, 0,
      (TEXTURE_WIDTH - frame.cols) / 2.0,
      (TEXTURE_HEIGHT - frame.rows) / 2.0,
      frame.cols, frame.rows,
      GL_RGB, GL_UNSIGNED_BYTE, frame.data);
}

void draw_pyramid() {
  GLdouble point0[] = {0.0, 1.0, 0.0};
  GLdouble pointA[] = {1.5, -1.0, 1.5};
  GLdouble pointB[] = {-1.5, -1.0, 1.5};
  GLdouble pointC[] = {-1.5, -1.0, -1.5};
  GLdouble pointD[] = {1.5, -1.0, -1.5};
  
  glColor3d(1.0, 0, 0);
  glBegin(GL_TRIANGLES);
  glVertex3dv(point0);
  glVertex3dv(pointA);
  glVertex3dv(pointB);
  glEnd();

  glColor3d(1.0, 1.0, 0);
  glBegin(GL_TRIANGLES);
  glVertex3dv(point0);
  glVertex3dv(pointB);
  glVertex3dv(pointC);
  glEnd();

  glColor3d(0, 1.0, 1.0);
  glBegin(GL_TRIANGLES);
  glVertex3dv(point0);
  glVertex3dv(pointC);
  glVertex3dv(pointD);
  glEnd();

  glColor3d(1.0, 0, 1.0);
  glBegin(GL_TRIANGLES);
  glVertex3dv(point0);
  glVertex3dv(pointD);
  glVertex3dv(pointA);
  glEnd();
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_TextureHandles[0]);
  glColor3d(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glTexCoord2d(1.0, 0.0);
  glVertex3dv(pointA);
  glTexCoord2d(0.0, 0.0);
  glVertex3dv(pointB);
  glTexCoord2d(0.0, 1.0);
  glVertex3dv(pointC);
  glTexCoord2d(1.0, 1.0);
  glVertex3dv(pointD);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}
