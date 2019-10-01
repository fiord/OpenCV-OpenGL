#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "test1"

void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);

void draw_square1();
void draw_square2();
void draw_square3();
void draw_hexagon();
void draw_poly(int);

// グローバル変数
int g_display_mode = 1;

int main(int argc, char *argv[]){
	/* OpenGLの初期化 */
  init_GL(argc, argv);

	/* このプログラム特有の初期化 */
  init();

	/* コールバック関数の登録 */
  set_callback_functions();

	/* メインループ */
	glutMainLoop();                              // 無限ループ。コールバック関数が呼ばれるまでずっと実行される。

	return 0;
}

void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);                              // OpenGLの初期化
	glutInitDisplayMode(GLUT_RGBA);                              // ディスプレイモードをRGBAモードに設定
	glutInitWindowSize(WINDOW_X, WINDOW_Y);                              // ウィンドウサイズを指定
	glutCreateWindow(WINDOW_NAME);                              // ウィンドウを「生成」。まだ「表示」はされない。
}

void init(){
	glClearColor(0, 0, 0, 0);                              // 背景の塗りつぶし色を指定
}

void set_callback_functions(){
	glutDisplayFunc(glut_display);                              // ディスプレイに変化があった時に呼ばれるコールバック関数を登録
	glutKeyboardFunc(glut_keyboard);                              // キーボードに変化があった時に呼び出されるコールバック関数を登録
}

// キーボードに変化があった時に呼び出されるコールバック関数。
void glut_keyboard(unsigned char key, int x, int y){
  static int num = 0;
  printf("called: %d\n", key);
	switch(key){

	case 'q':
	case 'Q':
	case '\033': // Escキーのこと
		exit(0);
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
    if (num < 10000) {
      num = num * 10 + key - '0';
    }
    printf("current num: %d\n", num);
    break;
  case  13:
    g_display_mode =  num;
    num = 0;
    break;
	}

	glutPostRedisplay();                  // 「ディスプレイのコールバック関数を呼んで」と指示する。
}

// ディスプレイに変化があった時に呼び出されるコールバック関数。
// 「ディスプレイに変化があった時」は、glutPostRedisplay() で指示する。
void glut_display(){
	glClear(GL_COLOR_BUFFER_BIT);                  // 今まで画面に描かれていたものを消す

	switch(g_display_mode){
	case 0:
		draw_square1();
		break;
	case 1:
		draw_square2();
		break;
	case 2:
		draw_square3();
		break;
  case 6:
    draw_hexagon();
  default:
    draw_poly(g_display_mode);
	}

	glFlush();                  // ここで画面に描画をする
}

void draw_square1(){
  glBegin(GL_LINE_LOOP);

  glColor3d(1, 0, 0);
  glVertex2d(-0.9, -0.9);
  glVertex2d(0.9, -0.9);
  glVertex2d(0.9, 0.9);
  glVertex2d(-0.9, 0.9);

  glEnd();
}

void draw_square2(){
  glBegin(GL_POLYGON);

  glColor3d(0, 1, 0);
  glVertex2d(-0.9, -0.9);
  glVertex2d(0.9, -0.9);
  glVertex2d(0.9, 0.9);
  glVertex2d(-0.9, 0.9);

  glEnd();
}

void draw_square3(){
  glBegin(GL_POLYGON);

  glColor3d(0, 0, 1);
  glVertex2d(-0.9, -0.9);
  glColor3d(1, 0, 0);
  glVertex2d(0.9, -0.9);
  glColor3d(0, 1, 0);
  glVertex2d(0.9, 0.9);
  glColor3d(0.5, 0.5, 0.5);
  glVertex2d(-0.9, 0.9);

  glEnd();
}

void draw_hexagon() {
  double radius = 0.9;
  double theta = 0;
  double dtheta = 2 * M_PI / 6.0;
  glBegin(GL_POLYGON);
  glColor3d(0, 0, 1);
  while(theta < 2 * M_PI) {
    glVertex2d(radius * cos(theta), radius * sin(theta));
    theta += dtheta;
  }
  glEnd();
}

void draw_poly(int n) {
  double radius = 0.9;
  double theta = 0;
  double dtheta = 2 * M_PI / n;
  glBegin(GL_POLYGON);
  glColor3d(1, 0, 0);
  while(theta < 2 * M_PI) {
    glVertex2d(radius * cos(theta), radius * sin(theta));
    theta += dtheta;
  }
  glEnd();
}
