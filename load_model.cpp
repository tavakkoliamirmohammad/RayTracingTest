#include "load_model.h"
#include <string>
#include <vector>
#include "OBJ_Loader.h"
#include <glm/glm.hpp>

void load_model(std::string path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals,
                std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &diffuse, std::vector<glm::vec3> &ambient,
                std::vector<glm::vec3> &specular) {
    // Initialize Loader
    objl::Loader Loader;

    // Load .obj File
    bool loadout = Loader.LoadFile(path);

    // Check to see if it loaded

    // If so continue
    if (loadout) {
        // Create/Open e1Out.txt
        std::ofstream file("e1Out.txt");

        // Go through each loaded mesh and out its contents
        for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
            // Copy one of the loaded meshes to be our current mesh
            objl::Mesh curMesh = Loader.LoadedMeshes[i];

            // Print Mesh Name
            file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

            // Print Vertices
            file << "Vertices:\n";

            // Go through each vertex and print its number,
            //  position, normal, and texture coordinate
            for (int j = 0; j < curMesh.Vertices.size(); j++) {
                file << "V" << j << ": " <<
                     "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", "
                     << curMesh.Vertices[j].Position.Z << ") " <<
                     "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", "
                     << curMesh.Vertices[j].Normal.Z << ") " <<
                     "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y
                     << ")\n";
            }

            // Print Indices
            file << "Indices:\n";

            // Go through every 3rd index and print the
            //	triangle that these indices represent
            for (int j = 0; j < curMesh.Indices.size(); j += 1) {
                auto vertex = curMesh.Vertices[curMesh.Indices[j]];
                vertices.emplace_back(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
                normals.emplace_back(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
                uvs.emplace_back(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y);
                auto material = curMesh.MeshMaterial;
                diffuse.emplace_back(material.Kd.X, material.Kd.Y, material.Kd.Z);
                ambient.emplace_back(material.Ka.X, material.Ka.Y, material.Ka.Z);
                specular.emplace_back(material.Ks.X, material.Ks.Y, material.Ks.Z);
            }

            // Print Material
            file << "Material: " << curMesh.MeshMaterial.name << "\n";
            file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", "
                 << curMesh.MeshMaterial.Ka.Z << "\n";
            file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", "
                 << curMesh.MeshMaterial.Kd.Z << "\n";
            file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", "
                 << curMesh.MeshMaterial.Ks.Z << "\n";
            file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
            file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
            file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
            file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
            file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
            file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
            file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
            file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
            file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

            // Leave a space to separate from the next mesh
            file << "\n";
        }

        // Close File
        file.close();
    }
        // If not output an error
    else {
        // Create/Open e1Out.txt
        std::ofstream file("e1Out.txt");

        // Output Error
        file << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";

        // Close File
        file.close();
    }
}