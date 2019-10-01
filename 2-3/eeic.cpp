#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test3"

void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);

void drawPlane();

// グローバル変数
double g_angle1 = 0.0;
double g_angle2 = 0.0;
double g_distance = 20.0;
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;

int main(int argc, char *argv[]){
	/* OpenGLの初期化 */
	init_GL(argc,argv);

	/* このプログラム特有の初期化 */
	init();

	/* コールバック関数の登録 */
	set_callback_functions();

	/* メインループ */
	glutMainLoop();

	return 0;
}


void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_X,WINDOW_Y);
	glutCreateWindow(WINDOW_NAME);
}

void init(){
	glClearColor(0.0, 0.0, 0.0, 0.0);         // 背景の塗りつぶし色を指定
}

void set_callback_functions(){
	glutDisplayFunc(glut_display);
	glutKeyboardFunc(glut_keyboard);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_motion);
	glutPassiveMotionFunc(glut_motion);
}

void glut_keyboard(unsigned char key, int x, int y){
	switch(key){
	case 'q':
	case 'Q':
	case '\033':
		exit(0);
	}
	glutPostRedisplay();
}

void glut_mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_UP){
			g_isLeftButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isLeftButtonOn = true;
		}
	}

	if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_UP){
			g_isRightButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isRightButtonOn = true;
		}
	}
}

void glut_motion(int x, int y){
	static int px = -1, py = -1;
	if(g_isLeftButtonOn == true){
		if(px >= 0 && py >= 0){
			g_angle1 += (double)-(x - px)/20;
			g_angle2 += (double)(y - py)/20;
		}
		px = x;
		py = y;
	}else if(g_isRightButtonOn == true){
		if(px >= 0 && py >= 0){
			g_distance += (double)(y - py)/20;
		}
		px = x;
		py = y;
	}else{
		px = -1;
		py = -1;
	}
	glutPostRedisplay();
}

void glut_display(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (cos(g_angle2)>0){
	gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
		g_distance * sin(g_angle2),
		g_distance * cos(g_angle2) * cos(g_angle1),
		0.0, 0.0, 0.0, 0.0, 1.0, 0.0);}
	else{
	gluLookAt(g_distance * cos(g_angle2) * sin(g_angle1),
                g_distance * sin(g_angle2),
                g_distance * cos(g_angle2) * cos(g_angle1),
                0.0, 0.0, 0.0, 0.0, -1.0, 0.0);}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

  glTranslatef(-3, -3, 0);

  glPushMatrix();
  glColor3f(1, 1, 1);

  drawPlane();
  glRotatef(-90, 0, 0, 1);
  glScalef(1, 0.75, 1);
  drawPlane();
  glTranslatef(-2.5, 0, 0);
  drawPlane();
  glTranslatef(-2.5, 0, 0);
  drawPlane();

  glPopMatrix();
  glTranslatef(5, 0, 0); 
  glPushMatrix();

  drawPlane();
  glRotatef(-90, 0, 0, 1);
  glScalef(1, 0.75, 1);
  drawPlane();
  glTranslatef(-2.5, 0, 0);
  drawPlane();
  glTranslatef(-2.5, 0, 0);
  drawPlane();

  glPopMatrix();
  glTranslatef(6, 0, 0);
  glPushMatrix();

  drawPlane();
  glRotatef(-90, 0, 0, 1);
  glTranslatef(0, -0.75, 0);
  glScalef(1, 0.5, 1);
  drawPlane();
  glTranslatef(-5, 0, 0);
  drawPlane();

  glPopMatrix();
  glTranslatef(3, 0, 0);
  glPushMatrix();

  drawPlane();
  glRotatef(-90, 0, 0, 1);
  glScalef(1, 0.75, 1);
  drawPlane();
  glTranslatef(-5, 0, 0);
  drawPlane();

  glPopMatrix();

	glFlush();
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void drawPlane() {
  GLdouble points[8][3] = {
    {0, 0, 0},
    {1, 0, 0},
    {1, 5, 0},
    {0, 5, 0},
    {0, 5, 1},
    {0, 0, 1},
    {1, 0, 1},
    {1, 5, 1},
  };
  int surfaces[6][4] = {
    {0, 1, 2, 3},
    {0, 1, 6, 5},
    {1, 2, 7, 6},
    {2, 3, 4, 7},
    {3, 0, 5, 4},
    {4, 5, 6, 7}
  };

  for (int i = 0; i < 6; i++) {
    int color = i + 1;
    glColor3d(color & 1, color & 2, color & 4);
    glBegin(GL_POLYGON);
    for(int j = 0; j < 4; j++) {
      glVertex3dv(points[surfaces[i][j]]);
    }
    glEnd();
  }
}
