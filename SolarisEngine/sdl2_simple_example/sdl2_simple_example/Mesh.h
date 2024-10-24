#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/mesh.h> // Para aiMesh

class Mesh {
public:
    // Constructor y destructor
    Mesh();
    ~Mesh();

    // Cargar malla desde aiMesh
    void LoadFromAssimp(aiMesh* ai_mesh);

    // Dibujar la malla
    void Draw();

    // Variables para almacenar los datos de la malla
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<GLuint> indices;
    GLuint vao, vbo, ebo;

private:
   

    // Identificadores de OpenGL
    

    // Configurar la malla
    void SetupMesh();
};

#endif // MESH_H
