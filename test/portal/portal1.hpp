#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include "common/Mesh.hpp"

#define NUM_PORTAL 2
#define GROUND_SIZE 20

glm::mat4 func1(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, 1, -2));
}
glm::mat4 func1_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, 1, 2)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func2(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -10 + 1, -2));
}
glm::mat4 func2_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -10 + 1, 2)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func3(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -20 + 1, -2));
}
glm::mat4 func3_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -20 + 1, 2)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 (*portal_init_pos_funcs[][2])(int,int) = {
  {func1, func2_rev},
  {func2, func1_rev},
  {func3, func3_rev}
};

std::set<std::pair<int, int>> target_portals[3][2];

std::vector<Mesh> ground, main_object;
Mesh light_bbox;
std::vector<Mesh>  portals[2];

void world_init() {
  main_object = std::vector<Mesh>(1);
  load_obj("cube.obj", &main_object[0]);
  main_object[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1 - 10, 0));

  target_portals[0][0].insert(std::make_pair(1, 1));
  target_portals[0][1].insert(std::make_pair(1, 0));
  target_portals[1][0].insert(std::make_pair(0, 1));
  target_portals[1][1].insert(std::make_pair(0, 0));
  target_portals[2][0].insert(std::make_pair(2, 1));
  target_portals[2][1].insert(std::make_pair(2, 0));
  
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

  ground = std::vector<Mesh>(3);
  for (int i = -GROUND_SIZE / 2; i < GROUND_SIZE / 2; i++) {
    for (int j = -GROUND_SIZE / 2; j < GROUND_SIZE / 2; j++) {
      ground[0].vertices.push_back(glm::vec4(i,    0.0, j + 1, 1.0));
      ground[0].vertices.push_back(glm::vec4(i + 1,  0.0, j + 1, 1.0));
      ground[0].vertices.push_back(glm::vec4(i,    0.0, j,   1.0));
      ground[0].vertices.push_back(glm::vec4(i,    0.0, j,   1.0));
      ground[0].vertices.push_back(glm::vec4(i + 1,  0.0, j + 1, 1.0));
      ground[0].vertices.push_back(glm::vec4(i + 1,  0.0, j,   1.0));

      for (int k = 0; k < 6; k++) {
        ground[0].normals.push_back(glm::vec3(0.0, 1.0, 0.0));
      }
    }
  }
  ground[0].color = {1, 0, 0};

  for (int i = -GROUND_SIZE / 2; i < GROUND_SIZE / 2; i++) {
    for (int j = -GROUND_SIZE / 2; j < GROUND_SIZE / 2; j++) {
      ground[1].vertices.push_back(glm::vec4(i,    -10.0, j + 1, 1.0));
      ground[1].vertices.push_back(glm::vec4(i + 1,  -10.0, j + 1, 1.0));
      ground[1].vertices.push_back(glm::vec4(i,    -10.0, j,   1.0));
      ground[1].vertices.push_back(glm::vec4(i,    -10.0, j,   1.0));
      ground[1].vertices.push_back(glm::vec4(i + 1,  -10.0, j + 1, 1.0));
      ground[1].vertices.push_back(glm::vec4(i + 1,  -10.0, j,   1.0));

      for (int k = 0; k < 6; k++) {
        ground[1].normals.push_back(glm::vec3(0.0, 1.0, 0.0));
      }
    }
  }
  ground[1].color = {0, 0, 1};

  for (int i = -GROUND_SIZE / 2; i < GROUND_SIZE / 2; i++) {
    for (int j = -GROUND_SIZE / 2; j < GROUND_SIZE / 2; j++) {
      ground[2].vertices.push_back(glm::vec4(i,    -20.0, j + 1, 1.0));
      ground[2].vertices.push_back(glm::vec4(i + 1,  -20.0, j + 1, 1.0));
      ground[2].vertices.push_back(glm::vec4(i,    -20.0, j,   1.0));
      ground[2].vertices.push_back(glm::vec4(i,    -20.0, j,   1.0));
      ground[2].vertices.push_back(glm::vec4(i + 1,  -20.0, j + 1, 1.0));
      ground[2].vertices.push_back(glm::vec4(i + 1,  -20.0, j,   1.0));

      for (int k = 0; k < 6; k++) {
        ground[2].normals.push_back(glm::vec3(0.0, 1.0, 0.0));
      }
    }
  }
  ground[2].color = {0, 1, 0};
}