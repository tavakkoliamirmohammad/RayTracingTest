#ifndef RAYTRACING_MATRIX_TRANSFORM_H
#define RAYTRACING_MATRIX_TRANSFORM_H

#include <glm/glm.hpp>

void computeMatricesFromInputs();

glm::mat4 getViewMatrix();

glm::mat4 getProjectionMatrix();

void keyboard_callback(unsigned char key, int x, int y);

void mouse_callback(int button, int state, int xpos, int ypos);

void special_keyboard_callback(int key, int x, int y);


#endif //RAYTRACING_MATRIX_TRANSFORM_H
