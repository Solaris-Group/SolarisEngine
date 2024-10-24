#include "Component_Mesh.h"
#include <iostream>

Component_Mesh::Component_Mesh(std::shared_ptr<GameObject> containerGO, std::shared_ptr<Mesh> mesh)
    : Component(containerGO, ComponentType::Mesh), // Asegúrate de pasar el tipo correcto aquí
    mesh(mesh) {
    // Inicialización adicional si es necesaria
}


Component_Mesh::~Component_Mesh() {}

void Component_Mesh::Enable() {
    enabled = true;
    // Lógica para habilitar el componente
}

void Component_Mesh::Disable() {
    enabled = false;
    // Lógica para deshabilitar el componente
}

void Component_Mesh::Update(double dt) {
    // Lógica de actualización
}

void Component_Mesh::DrawComponent() {
    if (!enabled || !mesh) {
        std::cerr << "El componente no está habilitado o el mesh es nulo." << std::endl;
        return;
    }

    //// Aplicar material si está presente
    //if (mesh->GetMaterial()) {
    //    mesh->GetMaterial()->Apply(); // Asegúrate de que esta función esté implementada
    //}

    // Dibujar el mesh
    mesh->Draw(); // Asumiendo que tienes un método Draw() en tu clase Mesh
}

void Component_Mesh::LoadMesh(aiMesh* ai_mesh) {
    // Configura tu VAO y VBO aquí
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, ai_mesh->mNumVertices * sizeof(aiVector3D), ai_mesh->mVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Guarda el VAO y otros datos necesarios para el dibujo
    this->vao = vao; // Asegúrate de tener un campo para almacenar esto
}
