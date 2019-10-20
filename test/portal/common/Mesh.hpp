#ifndef MESH_INCLUDED
#define MESH_INCLUDED

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <limits>

GLuint attribute_v_coord, attribute_v_normal;
GLuint uniform_m, uniform_v, uniform_p;
GLuint uniform_m_3x3_inv_transp, uniform_v_inv;

class Mesh {
  private:
    GLuint vbo_vertices, vbo_normals, ibo_elements;
  public:
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLushort> elements;
    glm::mat4 object2world;
    glm::vec3 min_point, max_point;
    glm::vec3 color;

    Mesh() : vbo_vertices(0), vbo_normals(0), ibo_elements(0), object2world(glm::mat4(1)), color(glm::vec3(1)) {}
    ~Mesh() {
      if (vbo_vertices != 0) {
        glDeleteBuffers(1, &vbo_vertices);
      }
      if (vbo_normals != 0) {
        glDeleteBuffers(1, &vbo_normals);
      }
      if (ibo_elements != 0) {
        glDeleteBuffers(1, &ibo_elements);
      }
    }

    // store object vertices, normals and/or elements in graphic card buffers
    void upload() {
      // set boundary
      float min_x, min_y, min_z, max_x, max_y, max_z;
      min_x = min_y = min_z = std::numeric_limits<float>::max();
      max_x = max_y = max_z = std::numeric_limits<float>::min();
      for (int i = 0; i < this->vertices.size(); i++) {
        min_x = std::min(min_x, this->vertices[i].x); max_x = std::max(max_x, this->vertices[i].x);
        min_y = std::min(min_y, this->vertices[i].y); max_y = std::max(max_y, this->vertices[i].y);
        min_z = std::min(min_z, this->vertices[i].z); max_z = std::max(max_z, this->vertices[i].z);
      }
      min_point = glm::vec3(min_x, min_y, min_z);
      max_point = glm::vec3(max_x, max_y, max_z);

      if (this->vertices.size() > 0) {
        glGenBuffers(1, &this->vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]), this->vertices.data(), GL_STATIC_DRAW);
      }

      if (this->normals.size() > 0) {
        glGenBuffers(1, &this->vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]), this->normals.data(), GL_STATIC_DRAW);
      }

      if (this->elements.size() > 0) {
        glGenBuffers(1, &this->ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(elements[0]), this->elements.data(), GL_STATIC_DRAW);
      }
    }

    // draw the object
    void draw() {
      if (this->vbo_vertices != 0) {
        glEnableVertexAttribArray(attribute_v_coord);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
        glVertexAttribPointer(
            attribute_v_coord,  // attribute
            4,                  // number of elements per vertex, here(x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
        );
      }

      if (this->vbo_normals != 0) {
        glEnableVertexAttribArray(attribute_v_normal);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
        glVertexAttribPointer(
            attribute_v_normal, // attribute
            3,                  // number of elements per vertex, here(x,y,z)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
        );
      }

      // apply object's transformation matrix
      glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(this->object2world));
      glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(this->object2world)));
      glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));

      glColor3d(color.r, color.g, color.b);
      // push each element in buffer_vertices to the vertex shader
      if (this->ibo_elements != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
      }
      else {
        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
      }

      if (this->vbo_normals != 0) {
        glDisableVertexAttribArray(attribute_v_coord);
      }
      if (this->vbo_vertices != 0) {
        glDisableVertexAttribArray(attribute_v_normal);
      }
    }

    // draw object bounding box
    void draw_bbox() {
      if (this->vertices.size() == 0) return;

      // cube 1x1x1, centered on origin
      GLfloat vertices[] = {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
      };
      GLuint vbo_vertices;
      glGenBuffers(1, &vbo_vertices);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
      };
      GLuint ibo_elements;
      glGenBuffers(1, &ibo_elements);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
      min_x = max_x = this->vertices[0].x;
      min_y = max_y = this->vertices[0].y;
      min_z = max_z = this->vertices[0].z;
      for(unsigned int i = 0; i < this->vertices.size(); i++) {
        if (this->vertices[i].x < min_x)  min_x = this->vertices[i].x;
        if (this->vertices[i].x > max_x)  max_x = this->vertices[i].x;
        if (this->vertices[i].y < min_y)  min_y = this->vertices[i].y;
        if (this->vertices[i].y > max_y)  max_y = this->vertices[i].y;
        if (this->vertices[i].z < min_z)  min_z = this->vertices[i].z;
        if (this->vertices[i].z > max_z)  max_z = this->vertices[i].z;
      }
      glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
      glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
      glm::mat4 transform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);

      // apply object's transformation matrix
      glm::mat4 m = this->object2world * transform;
      glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

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

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
      glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      glDisableVertexAttribArray(attribute_v_coord);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glDeleteBuffers(1, &vbo_vertices);
      glDeleteBuffers(1, &ibo_elements);
    }
};
#endif
