#ifndef RAYTRACING_LOAD_MODEL_H
#define RAYTRACING_LOAD_MODEL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

void load_model(std::string path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals,
                std::vector<glm::vec2> &uvs);

#endif //RAYTRACING_LOAD_MODEL_H
