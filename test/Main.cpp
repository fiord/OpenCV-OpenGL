#define DEBUG 1
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

#include "Player.hpp"

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test"

void init_GL(int, char**);
void init();
int readShaderSource(GLuint shader, const char *file);
void printShaderInfoLog(GLuint shader);
void printProgramInfoLog(GLuint program);
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_special(int key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_idle();

void create_room();

#define MAX_SHADER_LOG_SIZE (1024)
static GLuint vertShader;
static GLuint fragShader;
static GLuint gl2Program;

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
  // glutFullScreen();
  
  GLenum err = glewInit();
  if (err) {
    std::cerr << "failed in gl initialization" << std::endl;
    std::cerr << err << std::endl;
    std::cerr << glewGetErrorString(err) << std::endl;
    exit(1);
  }
  std::cerr << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  // shader
#if DEBUG
  GLint compiled, linked;
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (readShaderSource(vertShader, "vert.c")) exit(1);
  if (readShaderSource(fragShader, "frag.c")) exit(1);
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  std::cout << "OK" << std::endl;
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "compile error in vertex shader\n");
    exit(1);
  }
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if  (compiled == GL_FALSE) {
    fprintf(stderr, "compile error in fragment shader");
    exit(1);
  }
  gl2Program = glCreateProgram();
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    fprintf(stderr, "link error\n");
    exit(1);
  }
  glUseProgram(gl2Program);
#endif
}

void init() {
  glClearColor(0.2, 0.2, 0.2, 0.2);
  mainPlayer = new Player();
}

void printShaderInfoLog(GLuint shader) {
  int logSize;
  int length;
  GLchar *s_LogBuffer;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

  if (logSize > 1) {
    glGetShaderInfoLog(shader, MAX_SHADER_LOG_SIZE, &length, s_LogBuffer);
    fprintf(stderr, "shader infolog\n%s\n", s_LogBuffer);
  }
}

void printProgramInfoLog(GLuint program) {
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
  if (bufSize > 1) {
    GLchar *infoLog = (GLchar*) malloc(bufSize);
    if (infoLog == NULL) {
      std::cerr << "could not allocate infolog buffer\n" << std::endl;
    }
    else {
      GLsizei length;
      glGetProgramInfoLog(program, bufSize, &length, infoLog);
      fprintf(stderr, "infolog: \n%s\n", infoLog);
      free(infoLog);
    }
  }
}

int readShaderSource(GLuint shader, const char *file) {
  FILE *fp;
  const GLchar *source;
  GLsizei length;
  int ret;

  fp = fopen(file, "rb");
  if (fp == NULL) {
    std::cerr << "failed to open " << file << std::endl;
    exit(1);
  }
  
  fseek(fp, 0, SEEK_END);
  length = ftell(fp);

  source = (GLchar*)malloc(length);
  if (source == NULL) {
    std::cerr << "failed to allocate read buffer" << std::endl;
  }

  fseek(fp, 0, SEEK_SET);
  ret = fread((void*)source, 1, length, fp) != (size_t)length;
  fclose(fp);

  if (ret) {
    std::cerr << "failed to read file: " << file << std::endl;
    exit(1);
  }
  glShaderSource(shader, 1, &source, &length);

  free((void*)source);
  return ret;
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
      mainPlayer->changeVerocity(sin(mainPlayer->angle_2), 0, -cos(mainPlayer->angle_2));
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
      mainPlayer->changeVerocity(-sin(mainPlayer->angle_2), 0, cos(mainPlayer->angle_2));
      break;
    case ' ':
      mainPlayer->changeVerocity(0, 0.5, 0);
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
      0.0, 0.0, 0.0,
      0.0, 1.0, 0.0);

  /*
  GLfloat lightpos[] = {-9.0f, 9.0f, -9.0f, 1.0f};
  GLfloat diffuse[] = {1, 1, 1, 1};
  GLfloat ambient[] = {1, 1, 1, 1};
  */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  /*
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  */

  glPushMatrix();


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

  double rotated = rotate_cnt / 500.0;
  glColor3d(1, 0, 0);
  for (int i = 0; i < 8; i++) {
    if (i % 4 == 0) glColor3d(1, 0, 0);
    else if (i % 4 == 1)  glColor3d(0, 1, 0);
    else if (i % 4 == 2)  glColor3d(0, 0, 1);
    else if (i % 4 == 3)  glColor3d(1, 1, 0);

    glBegin(GL_POLYGON);
    GLdouble g_pos[3] = {
      (points[surfaces[i][0]][0] + points[surfaces[i][1]][0] + points[surfaces[i][2]][0]) / 3.0,
      (points[surfaces[i][0]][1] + points[surfaces[i][1]][1] + points[surfaces[i][2]][1]) / 3.0 - 1.0,
      (points[surfaces[i][0]][2] + points[surfaces[i][1]][2] + points[surfaces[i][2]][2]) / 3.0
    };
    GLdouble normal[3] = {
      g_pos[0] * cos(rotated) - g_pos[2] * sin(rotated),
      g_pos[1],
      g_pos[2] * cos(rotated) + g_pos[0] * sin(rotated)
    };
    glNormal3dv(normal);
    for (int j = 0; j < 3; j++) {
      GLdouble point[3] = {
        points[surfaces[i][j]][0] * cos(rotated) - points[surfaces[i][j]][2] * sin(rotated),
        points[surfaces[i][j]][1],
        points[surfaces[i][j]][2] * cos(rotated) + points[surfaces[i][j]][0] * sin(rotated)
      };
      glVertex3dv(point);
    }
    glEnd();
  }

  glPopMatrix();

  create_room();

  glFlush();

  /*
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHTING);
  */
  glDisable(GL_CULL_FACE);
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

  GLdouble normals[6][3] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 0},
    {0, 0, -1},
    {-1, 0, 0},
    {0, -1, 0} 
  };

  glColor3d(1, 1, 1);
  for (int i = 0; i < 6;  i++) {
    glBegin(GL_POLYGON);
    glNormal3dv(normals[i]);
    for (int j = 0; j < 4; j++) {
      glVertex3dv(points[surfaces[i][j]]);
    }
    glEnd();
  }
}
