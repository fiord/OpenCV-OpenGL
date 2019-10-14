/*
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
*/
#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "Mesh.hpp"

char *file_read(const char *filename) {
  FILE *in = fopen(filename, "rb");
  if (in == NULL) return NULL;

  int res_size = BUFSIZ;
  char *res = (char*)malloc(res_size);
  int nb_read_total = 0;

  while (!feof(in) && !ferror(in)) {
    if (nb_read_total + BUFSIZ > res_size) {
      if (res_size > 10 * 1024 * 1024)  break;
      res_size = res_size * 2;
      res = (char*)realloc(res, res_size);
    }
    char *p_res = res + nb_read_total;
    nb_read_total += fread(p_res, 1, BUFSIZ, in);
  }

  fclose(in);
  res = (char*)realloc(res, nb_read_total + 1);
  res[nb_read_total] = '\0';
  return res;
}

void print_log(GLuint object) {
  GLint log_length = 0;
  if (glIsShader(object)) {
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  }
  else if (glIsProgram(object)) {
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  }
  else {
    fprintf(stderr, "printlog: Not a shader or a program");
    return;
  }

  char *log = (char*)malloc(log_length);

  if (glIsShader(object)) {
    glGetShaderInfoLog(object, log_length, NULL, log);
  }
  else if (glIsProgram(object)) {
    glGetProgramInfoLog(object, log_length, NULL, log);
  }

  fprintf(stderr, "%s", log);
  free(log);
}

GLuint create_shader(const char *filename, GLenum type) {
  const GLchar *source = file_read(filename);
  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename);
    perror("");
    return 0;
  }

  GLuint res = glCreateShader(type);
  const GLchar *sources[] = {
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"
#else
    "#version 120\n"
#endif
    ,
#ifdef GL_ES_VERSION_2_0
    (type == GL_FRAGMENT_SHADER) ?
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "precision highp float;\n"
      "#else\n"
      "precision mediump float;\n"
      "#endif\n"
      : ""
#else
      "#define lowp\n"
      "#define mediump\n"
      "#define highp\n"
#endif
      , source
  };
  glShaderSource(res, 3, sources, NULL);
  free((void*)source);

  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    fprintf(stderr, "%s:", filename);
    print_log(res);
    glDeleteShader(res);
    return 0;
  }

  return res;
}

GLuint create_program(const char *vertex_file, const char *fragment_file) {
  GLuint program = glCreateProgram();
  GLuint shader;

  if (vertex_file) {
    shader = create_shader(vertex_file, GL_VERTEX_SHADER);
    if (!shader)  return 0;
    glAttachShader(program, shader);
  }

  if (fragment_file) {
    shader = create_shader(fragment_file, GL_FRAGMENT_SHADER);
    if (!shader)  return 0;
    glAttachShader(program, shader);
  }

  glLinkProgram(program);
  GLint link_ok = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram: ");
    print_log(program);
    glDeleteProgram(program);
    return 0;
  }

  return program;
}

#ifdef GL_GEOMETRY_SHADER
GLuint create_gs_program (const char *vertex_file, const char *geometry_file, const char *fragment_file, GLint input, GLint output, GLint vertices) {
  GLuint program = glCreateProgram();
  GLuint shader;

  if (vertex_file) {
    shader = create_shader(vertex_file, GL_VERTEX_SHADER);
    if (!shader)  return 0;
    glAttachShader(program, shader);
  }
  
  if (geometry_file) {
    shader = create_shader(geometry_file, GL_GEOMETRY_SHADER);
    if (!shader)  return 0;
    glAttachShader(program, shader);

    glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, input);
    glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, output);
    glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, vertices);
  }

  if (fragment_file) {
    shader = create_shader(fragment_file, GL_FRAGMENT_SHADER);
    if (!shader)  return 0;
    glAttachShader(program, shader);
  }

  glLinkProgram(program);
  GLint link_ok = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram: ");
    print_log(program);
    glDeleteProgram(program);
    return 0;
  }

  return program;
}
#else
GLuint create_gs_program(const char *vertex_file, const char *geometry_file, const char *fragment_file, GLuint input, GLuint output, GLuint vertices) {
  fprintf(stderr, "missing support for geometry shaders\n");
  return 0;
}
#endif

GLint get_attrib(GLuint program, const char *name) {
  GLint attribute = glGetAttribLocation(program, name);
  if (attribute == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", name);
  }
  return attribute;
}

GLint get_uniform(GLuint program, const char *name) {
  GLint uniform = glGetUniformLocation(program, name);
  if (uniform == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", name);
  }
  return uniform;
}

void load_obj(const char* filename, Mesh *mesh) {
  std::fstream ifs(filename, std::ios::in);
  if (!ifs) {
    std::cerr << "cannot open " << filename << std::endl;
    exit(1);
  }
  std::vector<int> nb_seen;

  std::string line;
  while (getline(ifs, line)) {
    if (line.substr(0, 2) == "v ") {
      std::istringstream s(line.substr(2));
      glm::vec4 v;  s >> v.x >> v.y >> v.z; v.w = 1.0;
      mesh->vertices.push_back(v);
    }
    else if (line.substr(0, 2) == "f ") {
      std::istringstream s(line.substr(2));
      GLushort a, b, c;
      s >> a >> b >> c; a--;  b--;  c--;
      mesh->elements.push_back(a);  mesh->elements.push_back(b);  mesh->elements.push_back(c);
    }
    else if (line[0] == '#') {
      // ignoring this line
    }
    else {
      // ignoring this line
    }
  }

  mesh->normals.resize(mesh->vertices.size(), glm::vec3(0, 0, 0));
  nb_seen.resize(mesh->vertices.size(), 0);
  for (unsigned int i = 0; i < mesh->elements.size(); i += 3) {
    GLushort ia = mesh->elements[i];
    GLushort ib = mesh->elements[i + 1];
    GLushort ic = mesh->elements[i + 2];
    glm::vec3 normal = glm::normalize(glm::cross(
          glm::vec3(mesh->vertices[ib]) - glm::vec3(mesh->vertices[ia]),
          glm::vec3(mesh->vertices[ic]) - glm::vec3(mesh->vertices[ia])
    ));

    int v[3]; v[0] = ia;  v[1] = ib;  v[2] = ic;
    for (int i = 0; i < 3; i++) {
      GLushort cur_v = v[i];
      nb_seen[cur_v]++;
      if (nb_seen[cur_v] == 1) {
        mesh->normals[cur_v] = normal;
      }
      else {
        // average
        mesh->normals[cur_v].x = mesh->normals[cur_v].x * (1.0 - 1.0 / nb_seen[cur_v]) + normal.x * 1.0 / nb_seen[cur_v];
        mesh->normals[cur_v].y = mesh->normals[cur_v].y * (1.0 - 1.0 / nb_seen[cur_v]) + normal.y * 1.0 / nb_seen[cur_v];
        mesh->normals[cur_v].z = mesh->normals[cur_v].z * (1.0 - 1.0 / nb_seen[cur_v]) + normal.z * 1.0 / nb_seen[cur_v];
      }
    }
  }
}

#endif
