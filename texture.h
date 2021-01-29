#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H
#include <GL/glew.h>

#include <GL/glut.h>

GLuint loadBMP_custom(const char *imagepath);

GLuint loadDDS(const char *imagepath);

#endif //RAYTRACING_TEXTURE_H
