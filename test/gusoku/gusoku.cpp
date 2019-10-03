#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <GL/glut.h>

#define WINDOW_NAME "Gusoku"

std::vector vertexIndices, uvIndices, normalIndices;
std::vector temp_vertices;
std::vector temp_uvs;
std::vector temp_normals;

void loadGusoku() {
  FILE * f = fopen("gusoku.obj", "r");
  if (f == NULL) {
    printf("failed to open gusoku.obj\n");
    exit(0);
  }

  while(true) {

  }
}
