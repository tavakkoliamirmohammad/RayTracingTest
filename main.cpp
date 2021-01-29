#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <vector>
#include "load_model.h"
#include "shader.h"
#include "texture.h"
#include "matrix_transform.h"

// Read our .obj file
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<glm::vec3> diffuse;
std::vector<glm::vec3> ambient;
std::vector<glm::vec3> specular;
std::vector<unsigned int> indices;
GLuint vertexbuffer, uvbuffer, normalbuffer, diffuseBuffer, ambientBuffer, specularBuffer, indexBuffer, programID, MatrixID, ViewMatrixID, ModelMatrixID, LightID, TextureID;


void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4, 4, 4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(TextureID, 0);
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    // 4th attribute buffer : diffuse
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseBuffer);
    glVertexAttribPointer(
            3,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    // 5th attribute buffer : ambient
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, ambientBuffer);
    glVertexAttribPointer(
            4,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    // 5th attribute buffer : specular
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, specularBuffer);
    glVertexAttribPointer(
            5,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);

    glutSwapBuffers();
}

void timer(int time) {
    computeMatricesFromInputs();
    glutPostRedisplay();
    glutTimerFunc(unsigned(20), timer, 0);
}

void init() {
// Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    glEnableClientState(GL_VERTEX_ARRAY);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("standardShader.vert", "standardShader.frag");

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");

    TextureID = glGetUniformLocation(programID, "myTextureSampler");
    load_model("car.obj", vertices, normals, uvs, diffuse, ambient, specular, indices);

    // Load it into a VBO

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    std::cout << indices.size() << std::endl;

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &diffuseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseBuffer);
    glBufferData(GL_ARRAY_BUFFER, diffuse.size() * sizeof(glm::vec3), &diffuse[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ambientBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ambientBuffer);
    glBufferData(GL_ARRAY_BUFFER, ambient.size() * sizeof(glm::vec3), &ambient[0], GL_STATIC_DRAW);

    glGenBuffers(1, &specularBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, specularBuffer);
    glBufferData(GL_ARRAY_BUFFER, specular.size() * sizeof(glm::vec3), &specular[0], GL_STATIC_DRAW);

    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1280, 768);

    glutCreateWindow("Press 1 to change color, 2 to increase vertex count");
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Glew error: %s\n", glewGetErrorString(err));
    }
    init();
    glutDisplayFunc(render);
//    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard_callback);
    glutSpecialFunc(special_keyboard_callback);
    glutTimerFunc(unsigned(20), timer, 0);

    glutMainLoop();
}