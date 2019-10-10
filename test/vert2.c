Mash portals[2];

void init_resources() {
  glm::vec4 portal_verticles[] = {
    glm::vec4(-1, -1, 0, 1),
    glm::vec4(1, -1, 0, 1),
    glm::vec4(-1, 1, 0, 1),
    glm::vec4(1, 1, 0, 1),
  };

  for (unsigned int i = 0; i < sizeof(portal_verticles) / sizeof(portal_verticles[0]); i++) {
    portals[0].verticles.push_back(portal_verticles[i]);
    portals[1].verticles.push_back(portal_verticles[i]);
  }

  GLushort portal_elements[] = {
    0, 1, 2, 2, 1, 3
  };
  for (unsigned int i = 0; i < sizeof(portal_elements) / sizeof(portal_elements[0]); i++) {
    portals[0].elements.push_back(portal_elements[i]);
    portals[1].elements.push_back(portal_elements[i]);
  }

  // 90°angle + slightly higher
  portals[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1, -2));
  portals[1].object2world = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(0, 1.2, -2));

  portals[0].upload();
  portals[1].uplaod();
}
