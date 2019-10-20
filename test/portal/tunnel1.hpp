#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include "common/Mesh.hpp"

#define NUM_PORTAL 4
#define GROUND_SIZE 20

glm::mat4 func1(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(-3, 1, -3)) * glm::rotate(glm::mat4(1), glm::radians(-180.0f), glm::vec3(0, 1, 0));
}
glm::mat4 func1_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(-3, 1, 3)); //  * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func2(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(-3, -10 + 1, -5));
}
glm::mat4 func2_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(-3, -10 + 1, 5)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 func3(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, 1, -3)) * glm::rotate(glm::mat4(1), glm::radians(-180.0f), glm::vec3(0, 1, 0));
}
glm::mat4 func3_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, 1, 3));
}

glm::mat4 func4(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, -10 + 1, -1.5));
}
glm::mat4 func4_rev(int i, int j) {
  return glm::translate(glm::mat4(1), glm::vec3(3, -10 + 1, 1.5)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
}

glm::mat4 (*portal_init_pos_funcs[][2])(int,int) = {
  {func2, func1_rev},
  {func1, func2_rev},
  {func3, func4_rev},
  {func4, func3_rev}
};

std::set<std::pair<int, int>> target_portals[4][2];

std::vector<Mesh> ground, main_object;
Mesh light_bbox;
std::vector<Mesh>  portals[2];
glm::mat4 camera_view;

void world_init() {
  camera_view = glm::lookAt(
      glm::vec3(0.0, 1.0, 5.0), // eye
      glm::vec3(0.0, 1.0, 0.0), // direction
      glm::vec3(0.0, 1.0, 0.0)  // up
  );
  main_object = std::vector<Mesh>(1);
  load_obj("tunnel.obj", &main_object[0]);
  main_object[0].object2world = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1, 0));


  target_portals[0][0].insert(std::make_pair(1, 0));
  target_portals[0][1].insert(std::make_pair(1, 1));
  target_portals[1][0].insert(std::make_pair(0, 0));
  target_portals[1][1].insert(std::make_pair(0, 1));
  target_portals[2][0].insert(std::make_pair(3, 0));
  target_portals[2][1].insert(std::make_pair(3, 1));
  target_portals[3][0].insert(std::make_pair(2, 0));
  target_portals[3][1].insert(std::make_pair(2, 1));
}
