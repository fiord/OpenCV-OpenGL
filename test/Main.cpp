#include <iostream>
#include <GL/glut.h>

#include "Player.hpp"

#define WINDOW_X (720)
#define WINDOW_Y (720)
#define WINDOW_NAME "test"

void init_GL(int, char**);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_special(int key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_idle();

void create_room();

bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
Player *mainPlayer;

// sample:rotating crystal
int rotate_cnt = 0;

int main(int argc, char *argv[]) {
  // initialize OpenGL
  init_GL(argc, argv);

  // initialization for this program
  init();

  // register callback functions
  set_callback_functions();

  // main loop
  glutMainLoop();
}

void init_GL(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}

void init() {
  glClearColor(0.2, 0.2, 0.2, 0.2);
  mainPlayer = new Player();
}

void set_callback_functions() {
  glutDisplayFunc(glut_display);
  glutKeyboardFunc(glut_keyboard);
  glutSpecialFunc(glut_special);
  glutMouseFunc(glut_mouse);
  glutMotionFunc(glut_motion);
  glutPassiveMotionFunc(glut_motion);
  glutIdleFunc(glut_idle);
}

void glut_keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case 'q':
    case 'Q':
    case '\033':
      exit(0);
    case 'a':
    case 'A':
      mainPlayer->changeVerocity(-sin(mainPlayer->angle_2), 0, cos(mainPlayer->angle_2));
      break;
    case 'w':
    case 'W':
      mainPlayer->changeVerocity(cos(mainPlayer->angle_2), 0, sin(mainPlayer->angle_2));
      break;
    case 's':
    case 'S':
      mainPlayer->changeVerocity(-cos(mainPlayer->angle_2), 0, -sin(mainPlayer->angle_2));
      break;
    case 'd':
    case 'D':
      mainPlayer->changeVerocity(sin(mainPlayer->angle_2), 0, -cos(mainPlayer->angle_2));
      break;
  }

  glutPostRedisplay();
}

void glut_special(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_LEFT:
      mainPlayer->changeVerocity(-1.0, 0, 0);
      break;
    case GLUT_KEY_UP:
      mainPlayer->changeVerocity(0, 0, 1.0);
      break;
    case GLUT_KEY_DOWN:
      mainPlayer->changeVerocity(0, 0, -1.0);
      break;
    case GLUT_KEY_RIGHT:
      mainPlayer->changeVerocity(1.0, 0, 0);
      break;
  }
  
  glutPostRedisplay();
}

void glut_mouse(int button, int state, int x, int y) {
  // 今の所何も処理をしません
}

void glut_motion(int x, int y) {
  static int px = -1, py = -1;
  if (px >= 0 && py >= 0) {
    mainPlayer->moveGaze((x - px), (y - py));
  }
  px = x;
  py = y;

  glutPostRedisplay();
}

void glut_idle() {
  rotate_cnt++;
  mainPlayer->movePosition();

  glutPostRedisplay();
}

void glut_display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, 1.0, 0.1, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  double target_x, target_y, target_z;
  mainPlayer->getTarget(target_x, target_y, target_z);
  gluLookAt(mainPlayer->pos_x, mainPlayer->pos_y, mainPlayer->pos_z,
      target_x, target_y, target_z,
      0.0, 1.0, 0.0);

  GLfloat lightpos[] = {-9.0f, 9.0f, -9.0f, 1.0f};
  GLfloat diffuse[] = {1, 1, 1, 1};
  GLfloat ambient[] = {0.2, 0.2, 0.2, 0.2};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  // glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

  glPushMatrix();
  glRotatef(rotate_cnt / 5.0, 0, 1, 0);
  static GLdouble points[6][3] = {
    {0, 2, 0},
    {1, 1, 0},
    {0, 1, 1},
    {-1, 1, 0},
    {0, 1, -1},
    {0, 0, 0}
  };
  static int surfaces[8][3] = {
    {0, 1, 2},
    {0, 2, 3},
    {0, 3, 4},
    {0, 4, 1},
    {1, 2, 5},
    {2, 3, 5},
    {3, 4, 5},
    {4, 1, 5}
  };

  glColor3d(1, 0, 0);
  for (int i = 0; i < 8; i++) {
    if (i % 4 == 0) glColor3d(1, 0, 0);
    else if (i % 4 == 1)  glColor3d(0, 1, 0);
    else if (i % 4 == 2)  glColor3d(0, 0, 1);
    else if (i % 4 == 3)  glColor3d(1, 1, 0);

    glBegin(GL_POLYGON);
    for (int j = 0; j < 3; j++) {
      glVertex3dv(points[surfaces[i][j]]);
    }
    glEnd();
  }

  glPopMatrix();

  create_room();

  glFlush();

  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

void create_room() {
  GLdouble points[8][3] = {
    {10.0, 0.0, -10.0},
    {-10.0, 0.0, -10.0},
    {-10.0, 0.0, 10.0},
    {10.0, 0.0, 10.0},
    {10.0, 20.0, -10.0},
    {-10.0, 20.0, -10.0},
    {-10.0, 20.0, 10.0},
    {10.0, 20.0, 10.0}
  };

  int surfaces[6][4] = {
    {0, 1, 2, 3},
    {0, 1, 5, 4},
    {1, 2, 6, 5},
    {2, 3, 7, 6},
    {3, 0, 4, 7},
    {4, 5, 6, 7}
  };

  glColor3d(1, 1, 1);
  for (int i = 0; i < 6;  i++) {
    glBegin(GL_POLYGON);
    for (int j = 0; j < 4; j++) {
      glVertex3dv(points[surfaces[i][j]]);
    }
    glEnd();
  }
}
