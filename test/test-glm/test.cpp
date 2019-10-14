#include <iostream>
#include <glm/glm.hpp>

auto main() -> int {
  auto a = glm::vec2(3, 4);
  std::cout << "a: " << a.x << " " << a.y << "\n" << "length(a): " << glm::length(a) << std::endl;
}
