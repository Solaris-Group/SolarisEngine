#include "Mesh.h"

// Constructor
Mesh::Mesh() : vao(0), vbo(0), ebo(0) {}

// Destructor
Mesh::~Mesh() {
    // Limpiar los recursos de OpenGL si es necesario
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
}

// Método para cargar el mesh desde aiMesh
void Mesh::LoadFromAssimp(aiMesh* ai_mesh) {
    vertices.resize(ai_mesh->mNumVertices);
    texCoords.resize(ai_mesh->mNumVertices);

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
        vertices[i] = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
        texCoords[i] = ai_mesh->mTextureCoords[0] ? glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
    }

    // No necesitas los índices si usas glDrawArrays, así que puedes omitir esta parte.
    // indices.reserve(ai_mesh->mNumFaces * 3);
    // for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
    //     const aiFace& face = ai_mesh->mFaces[i];
    //     for (unsigned int j = 0; j < face.mNumIndices; j++) {
    //         indices.push_back(face.mIndices[j]);
    //     }
    // }

    SetupMesh();
}

// Método para configurar el mesh (VAO, VBO)
void Mesh::SetupMesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Configurar atributos de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// Método para dibujar el mesh
void Mesh::Draw() {
    glBindVertexArray(vao);

    // Dibuja los vértices usando glDrawArrays
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3); // Aquí 'vertices.size()' da la cantidad de vértices.

    glBindVertexArray(0);
}
