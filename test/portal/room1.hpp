#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include "common/Mesh.hpp"

#define NUM_PORTAL 3
#define GROUND_SIZE 3
#define eps 0.01

glm::mat4 func1(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, 1, -3));
}
glm::mat4 func1_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func2(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -10 + 1, -3));
}
glm::mat4 func2_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, -10 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func3(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(0, -20 + 1, -3));
}
glm::mat4 func3_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, -20 + 1, 0)) * glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

glm::mat4 (*portal_init_pos_funcs[][2])(int,int) = {
  {func1, func2_rev},
  {func2, func3_rev},
  {func3, func1_rev}
};

std::set<std::pair<int, int>> target_portals[3][2];

std::vector<Mesh> ground, main_object;
Mesh light_bbox;
std::vector<Mesh>  portals[2];

const glm::vec3 base_colors[] = {
  {1, 0, 0},
  {0, 1, 0},
  {0, 0, 1}
};

void world_init() {
  main_object = std::vector<Mesh>(1);
  load_obj("cube.obj", &main_object[0]);
  main_object[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1 - 0, 0));

  target_portals[0][0].insert(std::make_pair(2, 1));
  target_portals[0][1].insert(std::make_pair(1, 0));
  target_portals[1][0].insert(std::make_pair(0, 1));
  target_portals[1][1].insert(std::make_pair(2, 0));
  target_portals[2][0].insert(std::make_pair(1, 1));
  target_portals[2][1].insert(std::make_pair(0, 0));
  
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

  ground = std::vector<Mesh>(15);
  for (int idx = 0, depth = 0; idx < 3; idx++, depth -= 10) {
    for (int i = -GROUND_SIZE; i < GROUND_SIZE; i++) {
      for (int j = -GROUND_SIZE; j < GROUND_SIZE; j++) {
        ground[5 * idx].vertices.push_back(glm::vec4(i,    depth, j + 1, 1.0));
        ground[5 * idx].vertices.push_back(glm::vec4(i + 1,  depth, j + 1, 1.0));
        ground[5 * idx].vertices.push_back(glm::vec4(i,    depth, j,   1.0));
        ground[5 * idx].vertices.push_back(glm::vec4(i,    depth, j,   1.0));
        ground[5 * idx].vertices.push_back(glm::vec4(i + 1,  depth, j + 1, 1.0));
        ground[5 * idx].vertices.push_back(glm::vec4(i + 1,  depth, j,   1.0));
        for (int k = 0; k < 6; k++) {
          ground[5 * idx].normals.push_back(glm::vec3(0.0, 1.0, 0.0));
        }
      }
    }
    for (int i = 0; i < 10; i++) {
      for (int j = -GROUND_SIZE; j < GROUND_SIZE; j++) {
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth,     j + 1, 1.0));
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth + i, j + 1, 1.0));
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth,     j,     1.0));
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth,     j,     1.0));
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth + i, j + 1, 1.0));
        ground[5 * idx + 1].vertices.push_back(glm::vec4(-GROUND_SIZE - eps, depth + i, j,     1.0));
        for (int k = 0; k < 6; k++) ground[5 * idx + 1].normals.push_back(glm::vec3(1.0, 0.0, 0.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth,     j + 1, 1.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth + i, j + 1, 1.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth,     j,     1.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth,     j,     1.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth + i, j + 1, 1.0));
        ground[5 * idx + 2].vertices.push_back(glm::vec4(GROUND_SIZE + eps, depth + i, j,     1.0));
        for (int k = 0; k < 6; k++) ground[5 * idx + 2].normals.push_back(glm::vec3(-1.0, 0.0, 0.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j + 1, depth,     GROUND_SIZE + eps, 1.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j + 1, depth + i, GROUND_SIZE + eps, 1.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j,     depth,     GROUND_SIZE + eps, 1.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j,     depth,     GROUND_SIZE + eps, 1.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j + 1, depth + i, GROUND_SIZE + eps, 1.0));
        ground[5 * idx + 3].vertices.push_back(glm::vec4(j,     depth + i, GROUND_SIZE + eps, 1.0));
        for (int k = 0; k < 6; k++) ground[5 * idx + 3].normals.push_back(glm::vec3(0.0, 0.0, -1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j + 1, depth,     -GROUND_SIZE - eps, 1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j + 1, depth + i, -GROUND_SIZE - eps, 1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j,     depth,     -GROUND_SIZE - eps, 1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j,     depth,     -GROUND_SIZE - eps, 1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j + 1, depth + i, -GROUND_SIZE - eps, 1.0));
        ground[5 * idx + 4].vertices.push_back(glm::vec4(j,     depth + i, -GROUND_SIZE - eps, 1.0));
        for (int k = 0; k < 6; k++) ground[5 * idx + 4].normals.push_back(glm::vec3(0.0, 0.0, 1.0));
      }
    }
    for (int i = 0; i < 5; i++) ground[5 * idx + i].color = base_colors[idx];
  }
}
