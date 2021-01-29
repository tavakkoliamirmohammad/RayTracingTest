#include "matrix_transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glut.h>
#include <iostream>

using namespace std;


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 direction;
glm::vec3 right1;
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second

void special_keyboard_callback(int key, int x, int y) {
    float rotate_speed = 0.1;
    if (key == GLUT_KEY_UP) {
        verticalAngle += rotate_speed;
    }
    // Move backward
    if (key == GLUT_KEY_DOWN) {
        verticalAngle -= rotate_speed;
    }
    // Strafe right
    if (key == GLUT_KEY_RIGHT) {
        horizontalAngle -= rotate_speed;

    }
    // Strafe left
    if (key == GLUT_KEY_LEFT) {
        horizontalAngle += rotate_speed;
    }
}

void keyboard_callback(unsigned char key, int x, int y) {
    // Move forward
    if (key == 'w') {
        position += direction * speed;
    }
    // Move backward
    if (key == 's') {
        position -= direction * speed;

    }
    // Strafe right
    if (key == 'd') {
        position += right1 * speed;

    }
    // Strafe left
    if (key == 'a') {
        position -= right1 * speed;
    }
}

void computeMatricesFromInputs() {

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    // Compute time difference between current and last frame
    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float deltaTime = float(currentTime - lastTime);

    direction = glm::vec3(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    right1 = glm::vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross(right1, direction);

    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    cout << position[0] << position[1] << position[2] << endl;
    // Camera matrix
    ViewMatrix = glm::lookAt(
            position,           // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
