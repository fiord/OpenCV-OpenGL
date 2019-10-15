#define DEBUG 0
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "common/shader_utils.hpp"
#include "common/Mesh.hpp"
#include "Player.hpp"
#include "portal1.hpp"

#define WINDOW_X (800)
#define WINDOW_Y (500)
#define WINDOW_NAME "test"

struct rect{ int x, y, w, h; };

void init_GL(int, char**);
bool init_function(char*, char*, char*);
void init_view();
int readShaderSource(GLuint shader, const char *file);
void printShaderInfoLog(GLuint shader);
void printProgramInfoLog(GLuint program);
void set_callback_functions();

void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_special(int key, int x, int y);
void glut_specialup(int key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_reshape(int width, int height);
void glut_idle();

void create_room();

// portal
void create_portal(Mesh*, int, int, float, float);
glm::mat4 portal_view(glm::mat4, Mesh*, Mesh*);
int portal_intersection(glm::vec4, glm::vec4, Mesh*);
void logic();
void fill_screen();
void draw_camera();
void draw_portal_bbox(Mesh*);
void draw_portal_stencil(std::vector<glm::mat4>, Mesh*);
bool clip_portal(std::vector<glm::mat4>, Mesh*, rect*);
void draw_portals(std::vector<glm::mat4>, int, int, int);
void draw_scene(std::vector<glm::mat4>, int, int, int);
void draw();

int screen_width = WINDOW_X, screen_height = WINDOW_Y;
GLuint program;
bool compute_arcball;
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int arcball_on = false;

enum MODES { MODE_OBJECT, MODE_CAMERA, MODE_LIGHT, MODE_LAST };
MODES view_mode = MODE_CAMERA;
int rotY_direction = 0, rotX_direction = 0, transZ_direction = 0, strife = 0;
float speed_factor = 1;
glm::mat4 transforms[MODE_LAST];
int last_ticks = 0;

static unsigned int fps_start = 0;
static unsigned int fps_frames = 0;
static float zNear = 0.01;
static float fovy = 45;

bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
Player *mainPlayer;

// sample:rotating crystal
int rotate_cnt = 0;

int main(int argc, char *argv[]) {
  // initialize OpenGL
  init_GL(argc, argv);

  // shader
  char *obj_filename = (char*)"cube.obj";
  char *v_shader_filename = (char*)"phong-shading.v.glsl";
  char *f_shader_filename = (char*)"phong-shading.f.glsl";
  if (argc == 4) {
    obj_filename = argv[1];
    v_shader_filename = argv[2];
    f_shader_filename = argv[3];
  }

  // initialization for this program
  if (init_function(obj_filename, v_shader_filename, f_shader_filename)) {
    init_view();
    // register callback functions
    set_callback_functions();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    // make bounding box clearer against the ground
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 0);
    last_ticks = glutGet(GLUT_ELAPSED_TIME);

    // main loop
    glutMainLoop();
  }

  glDeleteProgram(program);
}

void init_GL(int argc, char *argv[]) {
  glutInit(&argc, argv);
  // glutInitContextVersion(2,0);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow(WINDOW_NAME);
  // glutFullScreen();
  
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "failed in gl initialization" << std::endl;
    std::cerr << err << std::endl;
    std::cerr << glewGetErrorString(err) << std::endl;
    exit(1);
  }
  /*
  if (!GLEW_VERSION_2_0) {
    std::cerr << "Error: your graphic card does not support OpenGL 2.0" << std::endl;
    exit(1);
  }
  */
}

bool init_function(char *model_filename, char *vshader_filename, char *fshader_filename) {
  mainPlayer = new Player();

  load_obj(model_filename, &main_object);
  // mesh position initialized in init_view

  // world settings from portalX.hpp
  world_init();

  glm::vec3 light_position = glm::vec3(0.0, 1.0, 2.0);
  light_bbox.vertices.push_back(glm::vec4(-0.1, -0.1, -0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4( 0.1, -0.1, -0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4( 0.1,  0.1, -0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4(-0.1,  0.1, -0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4(-0.1, -0.1,  0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4( 0.1, -0.1,  0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4( 0.1,  0.1,  0.1, 0.0));
  light_bbox.vertices.push_back(glm::vec4(-0.1,  0.1,  0.1, 0.0));
  light_bbox.object2world = glm::translate(glm::mat4(1), light_position);

  for (int i = 0 ; i < 2; i++)  portals[i] = std::vector<Mesh>(NUM_PORTAL);
  for (int i = 0; i < portals[0].size(); i++) {
    create_portal(&portals[0][i], screen_width, screen_height, zNear, fovy);
    create_portal(&portals[1][i], screen_width, screen_height, zNear, fovy);

    // 90°angle
    portals[0][i].object2world = portal_init_pos_funcs[i][0](i, 0);
    portals[1][i].object2world = portal_init_pos_funcs[i][1](i, 1);
    portals[0][i].upload();
    portals[1][i].upload();
  }

  main_object.upload();
  for (int i = 0; i < ground.size(); i++) {
    ground[i].upload();
  }
  light_bbox.upload();

  // compile and link shaders
  GLint link_ok = GL_FALSE;
  GLint validate_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader(vshader_filename, GL_VERTEX_SHADER)) == 0)  return false;
  if ((fs = create_shader(fshader_filename, GL_FRAGMENT_SHADER)) == 0)  return false;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram: ");
    print_log(program);
    return false;
  }
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_ok);
  if (!validate_ok) {
    fprintf(stderr, "glValidateProgram: ");
    print_log(program);
  }

  const char *attribute_name;
  attribute_name = "v_coord";
  attribute_v_coord = glGetAttribLocation(program, attribute_name);
  if (attribute_v_coord == -1) {
    fprintf(stderr, "could not bind attribute %s\n", attribute_name);
    return false;
  }
  attribute_name = "v_normal";
  attribute_v_normal = glGetAttribLocation(program, attribute_name);
  if (attribute_v_normal == -1) {
    fprintf(stderr, "could not bind attribute %s\n", attribute_name);
    return false;
  }

  const char *uniform_name;
  uniform_name = "m";
  uniform_m = glGetUniformLocation(program, uniform_name);
  if (uniform_m == -1) {
    fprintf(stderr, "could not bind uniform %s\n", uniform_name);
    return false;
  }
  uniform_name = "v";
  uniform_v = glGetUniformLocation(program, uniform_name);
  if (uniform_v == -1) {
    fprintf(stderr, "could not bind uniform %s\n", uniform_name);
    return false;
  }
  uniform_name = "p";
  uniform_p = glGetUniformLocation(program, uniform_name);
  if (uniform_p == -1) {
    fprintf(stderr, "could not bind uniform %s\n", uniform_name);
    return false;
  }
  uniform_name = "m_3x3_inv_transp";
  uniform_m_3x3_inv_transp = glGetUniformLocation(program, uniform_name);
  if (uniform_m_3x3_inv_transp == -1) {
    fprintf(stderr, "could not bind uniform %s\n", uniform_name);
    return false;
  }
  uniform_name = "v_inv";
  uniform_v_inv = glGetUniformLocation(program, uniform_name);
  if (uniform_v_inv == -1) {
    fprintf(stderr, "could not bind uniform %s\n", uniform_name);
    return false;
  }

  fps_start = glutGet(GLUT_ELAPSED_TIME);
  return true;
}

void init_view() {
  main_object.object2world = glm::translate(glm::mat4(1), glm::vec3(-2, 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1, 0));
  transforms[MODE_CAMERA] = glm::lookAt(
      glm::vec3(0.0, 1.0, 6.0), // eye
      glm::vec3(0.0, 1.0, 0.0), // direction
      glm::vec3(0.0, 1.0, 0.0)  // up
  );
}

void set_callback_functions() {
  glutDisplayFunc(glut_display);
  glutKeyboardFunc(glut_keyboard);
  glutSpecialFunc(glut_special);
  glutSpecialUpFunc(glut_specialup);
  glutMouseFunc(glut_mouse);
  glutMotionFunc(glut_motion);
  glutPassiveMotionFunc(glut_motion);
  glutReshapeFunc(glut_reshape);
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
  int modifiers = glutGetModifiers();
  if ((modifiers && GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)  strife = 1;
  else  strife = 0;

  if ((modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT) speed_factor = 0.1;
  else  speed_factor = 1;

  switch(key) {
    case GLUT_KEY_F1:
      view_mode = MODE_OBJECT;
      break;
    case GLUT_KEY_F2:
      view_mode = MODE_CAMERA;
      break;
    case GLUT_KEY_F3:
      view_mode = MODE_LIGHT;
      break;
    case GLUT_KEY_LEFT:
      rotY_direction = 1;
      // mainPlayer->changeVerocity(-1.0, 0, 0);
      break;
    case GLUT_KEY_UP:
      transZ_direction = 1;
      // mainPlayer->changeVerocity(0, 0, 1.0);
      break;
    case GLUT_KEY_DOWN:
      transZ_direction = -1;
      // mainPlayer->changeVerocity(0, 0, -1.0);
      break;
    case GLUT_KEY_RIGHT:
      rotY_direction = -1;
      // mainPlayer->changeVerocity(1.0, 0, 0);
      break;
    case GLUT_KEY_HOME:
      init_view();
      break;
  }
  
  // glutPostRedisplay();
}

void glut_specialup(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
      rotY_direction = 0;
      break;
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
      transZ_direction = 0;
      break;
    case GLUT_KEY_PAGE_UP:
    case GLUT_KEY_PAGE_DOWN:
      rotX_direction = 0;
      break;
  }
}

void glut_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    arcball_on = true;
    last_mx = cur_mx = x;
    last_my = cur_my = y;
  }
  else {
    arcball_on = false;
  }
}

void glut_motion(int x, int y) {
  /*
  static int px = -1, py = -1;
  if (px >= 0 && py >= 0) {
    mainPlayer->moveGaze((x - px), (y - py));
  }
  px = x;
  py = y;

  glutPostRedisplay();
  */
  if (arcball_on) {
    cur_mx = x;
    cur_my = y;
  }
}

void glut_reshape(int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, screen_width, screen_height);
  for (int i = 0; i < portals[0].size(); i++) {
    create_portal(&portals[0][i], screen_width, screen_height, zNear, fovy);
    create_portal(&portals[1][i], screen_width, screen_height, zNear, fovy);
  }
}

void glut_idle() {
  // sample
  rotate_cnt++;

  glm::mat4 prev_cam = transforms[MODE_CAMERA];
  // update camera position depending on keyboard keys
  mainPlayer->movePosition();

  // handle portals
  // movement of the camera in world view
  for (int j = 0; j < portals[0].size(); j++) {
    for (int i = 0; i < 2; i++) {
      glm::vec4 la = glm::inverse(prev_cam) * glm::vec4(0.0, 0.0, 0.0, 1.0);
      glm::vec4 lb = glm::inverse(transforms[MODE_CAMERA]) * glm::vec4(0.0, 0.0, 0.0, 1.0);
      if (portal_intersection(la, lb, &portals[i][j])) {
        transforms[MODE_CAMERA] = portal_view(transforms[MODE_CAMERA], &portals[i][j], &portals[i^1][j]);
      }
    }
  }

  glutPostRedisplay();
}

void glut_display() {
  /*
  double target_x, target_y, target_z;
  mainPlayer->getTarget(target_x, target_y, target_z);
  gluLookAt(mainPlayer->pos_x, mainPlayer->pos_y, mainPlayer->pos_z,
      0.0, 0.0, 0.0,
      0.0, 1.0, 0.0);
  */
  logic();
  draw();
  glutSwapBuffers();
}

void create_portal(Mesh *portal, int screen_width, int screen_height,float zNear, float fovy) {
  portal->vertices.clear();
  portal->elements.clear();

  float aspect = 1.0 * screen_width / screen_height;
  float fovy_rad = fovy * M_PI / 180.0;
  float fovx_rad = fovy_rad / aspect;
  float dz = std::max(zNear / cos(fovx_rad), zNear / cos(fovy_rad));
  float dx = tan(fovx_rad) * dz;
  float dy = tan(fovy_rad) * dz;
  glm::vec4 portal_vertices[] = {
    glm::vec4(-1, -1, 0, 1),
    glm::vec4( 1, -1, 0, 1),
    glm::vec4(-1,  1, 0, 1),
    glm::vec4( 1,  1, 0, 1),

    glm::vec4(-(1+dx), -(1+dy), -dz, 1),
    glm::vec4( (1+dx), -(1+dy), -dz, 1),
    glm::vec4(-(1+dx),  (1+dy), -dz, 1),
    glm::vec4( (1+dx),  (1+dy), -dz, 1),
  };
  for (unsigned int i = 0; i < sizeof(portal_vertices) / sizeof(portal_vertices[0]); i++) {
    portal->vertices.push_back(portal_vertices[i]);
  }
  GLushort portal_elements[] = {
    0, 1, 2, 2, 1, 3,
    4, 5, 6, 6, 5, 7,
    0, 4, 2, 2, 4, 6,
    5, 1, 7, 7, 1, 3,
  };
  for (unsigned int i = 0; i < sizeof(portal_elements) / sizeof(portal_elements[0]); i++) {
    portal->elements.push_back(portal_elements[i]);
  }
}

// compute a world2camera view matrix to see from portal 'dst' given the original view and the 'src' portal position
glm::mat4 portal_view (glm::mat4 orig_view, Mesh* src, Mesh* dst) {
  glm::mat4 mv = orig_view * src->object2world;
  // 3. transformation from source portal to the camera - it's the first portal's modelview matrix
  // 2. object is front-facing, the camera is facing the other way
  // 1. go to the destination portal; using inverse, because camera transformations are reversed compared to object transformations
  glm::mat4 portal_cam = mv * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)) * glm::inverse(dst->object2world);
  return portal_cam;
}

// get a normalized vector from the center of the virtual ball O to a point P on the virtual ball surface, such that P is aligned on screen's (x, y) coordinates.
// if (x, y) is too far away from the sphere, return the nearest point on the virtual ball surface.
glm::vec3 get_arcball_vector(int x, int y) {
  glm::vec3 P = glm::vec3(
      1.0 * x / screen_width * 2.0 - 1.0,
      1.0 * y / screen_height * 2.0 - 1.0,
      0.0
  );
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1.0 * 1.0) {
    P.z = sqrt(1.0 * 1.0 - OP_squared);
  }
  else {
    P = glm::normalize(P);
  }

  return P;
}

// checks whether the line defined by two points la and lb intersects the portal
int portal_intersection(glm::vec4 la, glm::vec4 lb, Mesh* portal) {
  if (la != lb) { // if camera moved
    // check for intersection with each of the portal's 2 front triangles
    for (int i = 0; i < 2; i++) {
      // portal coordinates in world view
      glm::vec4 p0 = portal->object2world * portal->vertices[portal->elements[i * 3 + 0]];
      glm::vec4 p1 = portal->object2world * portal->vertices[portal->elements[i * 3 + 1]];
      glm::vec4 p2 = portal->object2world * portal->vertices[portal->elements[i * 3 + 2]];

      // solve line-plane intersection using parametric form
      glm::vec3 tuv = glm::inverse(glm::mat3(
            glm::vec3(la.x - lb.x, la.y - lb.y, la.z - lb.z),
            glm::vec3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z),
            glm::vec3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z)
            )) * glm::vec3(la.x - p0.x, la.y - p0.y, la.z - p0.z);
      float t = tuv.x, u = tuv.y, v = tuv.z;

      // intersection with the plane
      if (t >= -1e-6 && t <= 1 + 1e-6) {
        // intersection with the triangle
        if (u >= -1e-6 && u <= 1 + 1e-6 && v >= -1e-6 && v <= 1 + 1e-6 && (u + v) <= 1 + 1e-6) {
          return 1;
        }
      }
    }
  }
  return 0;
}

void logic() {
  // fps check
  {
    fps_frames++;
    int delta_t = glutGet(GLUT_ELAPSED_TIME) - fps_start;
    if (delta_t > 1000) {
      if (1000.0 * fps_frames / delta_t < 30.0) {
        std::cout << 1000.0 * fps_frames / delta_t << std::endl;
      }
      fps_frames = 0;
      fps_start = glutGet(GLUT_ELAPSED_TIME);
    }
  }

  glm::mat4 prev_cam = transforms[MODE_CAMERA];

  // handle keyboard-based transformations
  int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
  last_ticks = glutGet(GLUT_ELAPSED_TIME);

  float delta_transZ = transZ_direction * delta_t / 1000.0 * 5.0 * speed_factor; // 5 units per second
  float delta_transX = 0, delta_transY = 0, delta_rotY = 0, delta_rotX = 0;
  if (strife) {
    delta_transX = rotY_direction * delta_t / 1000.0 * 3.0 * speed_factor; // 3 units per second
    delta_transY = rotX_direction * delta_t / 1000.0 * 3.0 * speed_factor; // 3 units per second
  }
  else {
    delta_rotY = rotY_direction * delta_t / 1000.0 * 120 * speed_factor; // 120°per second
    delta_rotX = rotX_direction * delta_t / 1000.0 * 120 * speed_factor; // 120°per second
  }

  if (view_mode == MODE_OBJECT) {
    main_object.object2world = glm::rotate(main_object.object2world, glm::radians(delta_rotY), glm::vec3(0.0, 1.0, 0.0));
    main_object.object2world = glm::rotate(main_object.object2world, glm::radians(delta_rotX), glm::vec3(1.0, 0.0, 0.0));
    main_object.object2world = glm::translate(main_object.object2world, glm::vec3(0.0, 0.0, delta_transZ));
  }
  else if (view_mode == MODE_CAMERA) {
    // camera is reverse-facing, so reverse Z translation and X rotation.
    // in addition, the view matrix is the inverse of the camera2world(it's world->camera), so we'll reverse the transformations.
    // alternatively, imagine that you transform the world, instead of positioning the camera.
    if (strife) {
      transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(delta_transX, 0.0, 0.0)) * transforms[MODE_CAMERA];
    }
    else {
      glm::vec3 y_axis_world = glm::mat3(transforms[MODE_CAMERA]) * glm::vec3(0.0, 1.0, 0.0);
      transforms[MODE_CAMERA] = glm::rotate(glm::mat4(1.0), glm::radians(-delta_rotY), y_axis_world) * transforms[MODE_CAMERA];
    }

    if (strife) {
      transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(0.0, delta_transY, 0.0)) * transforms[MODE_CAMERA];
    }
    else {
      transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, delta_transZ)) * transforms[MODE_CAMERA];
    }

    transforms[MODE_CAMERA] = glm::rotate(glm::mat4(1.0), glm::radians(delta_rotX), glm::vec3(1.0, 0.0, 0.0)) * transforms[MODE_CAMERA];
  }

  // handle portals
  // movement of the camera in world view
  for (int j = 0; j < portals[0].size(); j++) {
    for (int i = 0; i < 2; i++) {
      glm::vec4 la = glm::inverse(prev_cam) * glm::vec4(0.0, 0.0, 0.0, 1.0);
      glm::vec4 lb = glm::inverse(transforms[MODE_CAMERA]) * glm::vec4(0.0, 0.0, 0.0, 1.0);
      if (portal_intersection(la, lb, &portals[i][j])) {
        transforms[MODE_CAMERA] = portal_view(transforms[MODE_CAMERA], &portals[i][j], &portals[i^1][j]);
      }
    }
  }

  // handle arcball
  if (cur_mx != last_mx || cur_my != last_my) {
    glm::vec3 va = get_arcball_vector(last_mx, last_my);
    glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
    float angle = acos(std::min(1.0f, glm::dot(va, vb)));
    glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
    glm::mat3 camera2object = glm::inverse(glm::mat3(transforms[MODE_CAMERA]) * glm::mat3(main_object.object2world));
    glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
    main_object.object2world = glm::rotate(main_object.object2world, angle, axis_in_object_coord);
    last_mx = cur_mx;
    last_my = cur_my;
  }
  
  // view
  glm::mat4 world2camera = transforms[MODE_CAMERA];

  // projection
  glm::mat4 camera2screen = glm::perspective(fovy, 1.0f * screen_width / screen_height, zNear, 100.0f);
  
  glUseProgram(program);
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(world2camera));
  glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(camera2screen));

  glm::mat4 v_inv = glm::inverse(world2camera);
  glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

  glutPostRedisplay();
}

// fill screen with a black square aligned with the perspective
void fill_screen() {
  GLfloat vertices[] = {
    -1, -1, 0, 1,
     1, -1, 0, 1,
    -1,  1, 0, 1,
    -1,  1, 0, 1,
     1, -1, 0, 1,
     1,  1, 0, 1,
  };
  GLuint vbo_vertices;
  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // apply object's transformation matrix
  glm::mat4 m = glm::inverse(glm::perspective(fovy, 1.0f * screen_width / screen_height, zNear, 100.0f));
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

  // save current view matrix - useful since we're going to recursively draw the scene from different points of view
  GLuint cur_program;
  GLfloat save_v[16];
  GLfloat save_v_inv[16];
  glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&cur_program);
  glGetUniformfv(cur_program, uniform_v, save_v);
  glGetUniformfv(cur_program, uniform_v_inv, save_v_inv);

  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glEnableVertexAttribArray(attribute_v_coord);
  glVertexAttribPointer(
      attribute_v_coord,  // attribute
      4,                  // number of elements per vertex, here(x,y,z,w)
      GL_FLOAT,           // the type of each element
      GL_FALSE,           // take our values as-is
      0,                  // no extra data between each position
      0                   // offset of first element
  );

  glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(vertices[0]) / 4.0);
  glDisableVertexAttribArray(attribute_v_coord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo_vertices);
  
  // restore view matrix
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, save_v);
  glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, save_v_inv);
}

void draw_camera() {
  GLfloat vertices[] = {
    -1, -1, 0, 1,
     1, -1, 0, 1,
     1, -1, 0, 1,
    -1,  1, 0, 1,
    -1,  1, 0, 1,
    -1, -1, 0, 1,
    -1,  1, 0, 1,
     1, -1, 0, 1,
     1, -1, 0, 1,
     1,  1, 0, 1,
     1,  1, 0, 1,
    -1,  1, 0, 1,
  };
  GLuint vbo_vertices;
  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // apply object's transformation matrix
  glm::mat4 m = glm::inverse(transforms[MODE_CAMERA]);
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glEnableVertexAttribArray(attribute_v_coord);
  glVertexAttribPointer(
      attribute_v_coord,  // attribute
      4,                  // number of elements per vertex, here(x,y,z,w)
      GL_FLOAT,           // the type of each element
      GL_FALSE,           // take our values as-is
      0,                  // no extra data between each position
      0                   // offsetof first element
  );

  glDrawArrays(GL_LINES, 0, 6);
  glDrawArrays(GL_LINES, 6, 6);

  glDisableVertexAttribArray(attribute_v_coord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo_vertices);
}

// draw a frame around the portal
void draw_portal_bbox(Mesh *portal) {
  // 0.05 frame around the portal
  Mesh portal_bbox;
  portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.05, -1.05, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4(-1.05, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.05,  1.05, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4( 1.05, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.05,  1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.05,  1.00, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.00, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.00, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.00, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.00, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.00, 0, 1));

  portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.00, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
  portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));

  for (int i = 0; i < portal_bbox.vertices.size(); i++) {
    portal_bbox.normals.push_back(glm::vec3(0, 0, 1));
  }

  portal_bbox.upload();

  // apply object's transformation matrix
  portal_bbox.object2world = portal->object2world;
  portal_bbox.draw();
  portal_bbox.object2world = portal->object2world * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
  portal_bbox.draw();
}

void draw_portal_stencil(std::vector<glm::mat4> view_stack, Mesh* portal) {
  GLboolean save_color_mask[4];
  GLboolean save_depth_mask;
  glGetBooleanv(GL_COLOR_WRITEMASK, save_color_mask);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &save_depth_mask);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  glStencilFunc(GL_NEVER, 0, 0xff);
  glStencilOp(GL_INCR, GL_KEEP, GL_KEEP); // draw 1s on test fail(always)

  
  // draw stencil pattern
  glClear(GL_STENCIL_BUFFER_BIT); // needs mask=0xFF
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[0]));
  portal->draw();

  for (unsigned int i = 1; i < view_stack.size() - 1; i++) {  // ignore last view
    // increment intersection for current portal
    glStencilFunc(GL_EQUAL, 0, 0xff);
    glStencilOp(GL_INCR, GL_KEEP, GL_KEEP); // draw 1s on test fail(always)
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[i]));
    portal->draw();

    // decremental outer portal -> only sub-portal intersection remains
    glStencilFunc(GL_NEVER, 0, 0xff);
    glStencilOp(GL_DECR, GL_KEEP, GL_KEEP); // draw 1s on test fail(always)
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[i-1]));
    portal->draw();
  }

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  // fill 1 or more
  glStencilFunc(GL_LEQUAL, 1, 0xff);
  glColorMask(save_color_mask[0], save_color_mask[1], save_color_mask[2], save_color_mask[3]);
  glDepthMask(save_depth_mask);
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack.back()));
  // now ready for drawing main scene.
}

bool clip_portal(std::vector<glm::mat4> view_stack, Mesh *outer_portal, rect *scissor) {
  scissor->x = scissor->y = 0;
  scissor->w = screen_width;
  scissor->h = screen_height;
  for(unsigned int v = 0; v < view_stack.size() - 1; v++) { // ignore the last view
    glm::vec4 p[4];
    rect *r = new rect;
    bool found_negative_w = false;
    for (int pi = 0; pi < 4; pi++) {
      p[pi] = (glm::perspective(fovy, 1.0f * screen_width / screen_height, zNear, 100.0f) * view_stack[v] * outer_portal->object2world) * outer_portal->vertices[pi];
      if (p[pi].w < 0) {
        // TODO
        found_negative_w = true;
      }
      else {
        p[pi].x /= p[pi].w;
        p[pi].y /= p[pi].w;
      }
    }
    if (found_negative_w) continue;

    glm::vec4 min_x, max_x, min_y, max_y;
    min_x = max_x = min_y = max_y = p[0];
    for (int i = 0; i < 4; i++) {
      if (p[i].x < min_x.x) min_x = p[i];
      if (p[i].x > max_x.x) max_x = p[i];
      if (p[i].y < min_y.y) min_y = p[i];
      if (p[i].y > max_y.y) max_y = p[i];
    }

    min_x.x = (std::max(-1.0f, min_x.x) + 1) / 2 * screen_width;
    max_x.x = (std::min(1.0f, max_x.x) + 1) / 2 * screen_width;
    min_y.y = (std::max(-1.0f, min_y.y) + 1) / 2 * screen_height;
    max_y.y = (std::min(1.0f, max_y.y) + 1) / 2 * screen_height;

    r->x = min_x.x;  r->y = min_y.y;
    r->w = max_x.x - min_x.x;  r->h = max_y.y - min_y.y;

    {
      int r_min_x = std::max(r->x, scissor->x);
      int r_max_x = std::min(r->x + r->w, scissor->x + scissor->w);
      int r_min_y = std::max(r->y, scissor->y);
      int r_max_y = std::min(r->y + r->h, scissor->y + scissor->h);
      scissor->x = r_min_x; scissor->y = r_min_y;
      scissor->w = r_max_x - r_min_x; scissor->h = r_max_y - r_min_y;
    }

    if (scissor->w <= 0 || scissor->h <= 0) {
      return false;
    }
  }
  return true;
}

void draw_portals(std::vector<glm::mat4> view_stack, int rec, int outer_portal, int portal_set) {
  // TODO: replace rec with size threshold for MV * portal.bbox?

  GLboolean save_stencil_test;
  glGetBooleanv(GL_STENCIL_TEST, &save_stencil_test);

  glEnable(GL_STENCIL_TEST);
  glEnable(GL_SCISSOR_TEST);
  for (int j = 0; j < portals[0].size(); j++) {
    for (int i = 0; i < 2; i++) {
      // Important: don't draw outer_portal's outgoing portal - only draw the same portqal when displaying a sub-portal (seen from the other portal)
      if ((outer_portal == -1 && portal_set == -1) || (i == outer_portal || j == portal_set)) {
        glm::mat4 portal_cam = portal_view(view_stack.back(), &portals[i][j], &portals[i^1][j]);
        view_stack.push_back(portal_cam);
        draw_scene(view_stack, rec + 1, i, j);
        view_stack.pop_back();
        glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack.back()));
        glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(glm::inverse(view_stack.back())));
        // TODO: write something without lines. I don't have confidence in its interaction with the stencil buffer
      }
    }
  }
  if (!save_stencil_test) {
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
  }

  // draw portal in the depth buffer so they are not overwritten
  glClear(GL_DEPTH_BUFFER_BIT);

  GLboolean save_color_mask[4];
  GLboolean save_depth_mask;
  glGetBooleanv(GL_COLOR_WRITEMASK, save_color_mask);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &save_depth_mask);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_TRUE);
  for (int j = 0; j < portals[0].size(); j++) {
    for (int i = 0; i < 2; i++) {
      portals[i][j].draw();
    }
  }
  glColorMask(save_color_mask[0], save_color_mask[1], save_color_mask[2], save_color_mask[3]);
  glDepthMask(save_depth_mask);
}

void draw_scene(std::vector<glm::mat4> view_stack, int rec, int outer_portal = -1, int portal_set = -1) {
  if (rec >= 5) {
    // TODO?
    return;
  }
  rect *scissor = new rect;
  if (outer_portal != -1 && portal_set != -1) {
    // if basic clipping returns an empty rectangle, we can stop here
    if (!clip_portal(view_stack, &portals[outer_portal][portal_set], scissor)) {
      return;
    }
  }

  // set view matrix
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack.back()));
  glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(glm::inverse(view_stack.back())));

  glClear(GL_DEPTH_BUFFER_BIT);

  // draw portals contents
  draw_portals(view_stack, rec, outer_portal, portal_set);

  if (outer_portal != -1 && portal_set != -1) {
    // clip the current view as mush as possible, more efficient than using the stencil buffer
    glScissor(scissor->x, scissor->y, scissor->w, scissor->h);

    // draw the current stencil - or actually recreate it if we just drew a sub-portal and hence messed the stencil buffer
    draw_portal_stencil(view_stack, &portals[outer_portal][portal_set]);
  }

  // draw portals frames after the stencil buffer is set
  for (int j = 0; j < portals[0].size(); j++) {
    for (int i = 0; i < 2; i++) {
      draw_portal_bbox(&portals[i][j]);
    }
  }

  // draw scene
  main_object.draw();
  for (int i = 0; i < ground.size(); i++) {
    ground[i].draw();
  }

  // restore view matrix
  view_stack.pop_back();
  if (view_stack.size() > 0) {
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack.back()));
    glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(glm::inverse(view_stack.back())));
  }
}

void draw() {
  glClearColor(0.45, 0.45, 0.45, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glUseProgram(program);

  std::vector<glm::mat4> view_stack;
  view_stack.push_back(transforms[MODE_CAMERA]);

  glViewport(0, 0, screen_width, screen_height);
  draw_scene(view_stack, 1);

  glViewport(2 * screen_width / 3, 0, screen_width / 3, screen_height / 3);
  glClear(GL_DEPTH_BUFFER_BIT);
  view_stack.clear();
  view_stack.push_back(glm::lookAt(
        glm::vec3(0.0, 9.0, -2.0),  // eye
        glm::vec3(0.0, 0.0, -2.0),  // direction
        glm::vec3(0.0, 0.0, -1.0)   // up
  ));
  draw_scene(view_stack, 4);
  draw_camera();
}
