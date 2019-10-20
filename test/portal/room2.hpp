#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include "common/Mesh.hpp"

#define NUM_PORTAL 6
#define GROUND_SIZE 4
const float eps = 0.01;

glm::mat4 func1(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, 1, -2));
}
glm::mat4 func1_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func2(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -10 + 1, -2));
}
glm::mat4 func2_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, -10 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func3(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -20 + 1, -2));
}
glm::mat4 func3_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, -20 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func4(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -30 + 1, -2));
}
glm::mat4 func4_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, -30 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func5(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -40 + 1, -2));
}
glm::mat4 func5_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, -40 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func6(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -50 + 1, -2));
}
glm::mat4 func6_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(2, -50 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 (*portal_init_pos_funcs[][2])(int,int) = {
  {func1, func2_rev},
  {func2, func3_rev},
  {func3, func4_rev},
  {func4, func5_rev},
  {func5, func6_rev},
  {func6, func1_rev},
};

std::set<std::pair<int, int>> target_portals[6][2];

std::vector<Mesh> ground, main_object;
Mesh light_bbox;
std::vector<Mesh>  portals[2];

glm::vec3 base_color[] = {
  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 1.0, 0.0},
  {0.0, 1.0, 1.0},
  {1.0, 0.0, 1.0}
};
glm::mat4 camera_view;

void world_init() {
  camera_view = glm::lookAt(
      glm::vec3(0.0, 1.0, 1.5), // eye
      glm::vec3(0.0, 1.0, 0.0), // direction
      glm::vec3(0.0, 1.0, 0.0)  // up
  );
  main_object = std::vector<Mesh>(1);
  // load_obj("cube.obj", &main_object[0]);
  main_object[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1, 0));

  target_portals[0][0].insert(std::make_pair(1, 0));
  target_portals[0][1].insert(std::make_pair(5, 1));
  target_portals[1][0].insert(std::make_pair(2, 0));
  target_portals[1][1].insert(std::make_pair(0, 1));
  target_portals[2][0].insert(std::make_pair(3, 0));
  target_portals[2][1].insert(std::make_pair(1, 1));
  target_portals[3][0].insert(std::make_pair(4, 0));
  target_portals[3][1].insert(std::make_pair(2, 1));
  target_portals[4][0].insert(std::make_pair(5, 0));
  target_portals[4][1].insert(std::make_pair(3, 1));
  target_portals[5][0].insert(std::make_pair(0, 0));
  target_portals[5][1].insert(std::make_pair(4, 1));
  
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

  ground = std::vector<Mesh>(18);
  for (int idx = 6; idx < 6; idx++) {
    for (int i = -GROUND_SIZE / 2; i < GROUND_SIZE / 2; i++) {
      for (int j = -GROUND_SIZE / 2; j < GROUND_SIZE / 2; j++) {
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i,      -idx * 10.0, j + 1, 1.0));
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i + 1,  -idx * 10.0, j + 1, 1.0));
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i,      -idx * 10.0, j,     1.0));
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i,      -idx * 10.0, j,     1.0));
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i + 1,  -idx * 10.0, j + 1, 1.0));
        ground[3 * idx + 0].vertices.push_back(glm::vec4(i + 1,  -idx * 10.0, j,     1.0));

        for (int k = 0; k < 6; k++) {
          ground[3 * idx + 0].normals.push_back(glm::vec3(0.0, 1.0, 0.0));
        }
      }
    }
    ground[3 * idx + 0].color = base_color[idx];
    for (int i = 0; i < 8; i++) {
      for (int j = -GROUND_SIZE / 2; j < GROUND_SIZE / 2; j++) {
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i,     j + 1, 1.0));
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i + 1, j + 1, 1.0));
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i,     j,     1.0));
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i,     j,     1.0));
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i + 1, j + 1, 1.0));
        ground[3 * idx + 1].vertices.push_back(glm::vec4( GROUND_SIZE/2.0 + eps, -idx * 10.0 + i + 1, j,     1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j + 1, -idx * 10.0 + i,     -GROUND_SIZE/2.0 - eps, 1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j + 1, -idx * 10.0 + i + 1, -GROUND_SIZE/2.0 - eps, 1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j,     -idx * 10.0 + i,     -GROUND_SIZE/2.0 - eps, 1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j,     -idx * 10.0 + i,     -GROUND_SIZE/2.0 - eps, 1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j + 1, -idx * 10.0 + i + 1, -GROUND_SIZE/2.0 - eps, 1.0));
        ground[3 * idx + 2].vertices.push_back(glm::vec4(j,     -idx * 10.0 + i + 1, -GROUND_SIZE/2.0 - eps, 1.0));
      }
    }
  }
}
