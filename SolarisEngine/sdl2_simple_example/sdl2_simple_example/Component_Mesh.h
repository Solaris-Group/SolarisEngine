#pragma once
#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <memory> // Asegúrate de incluir esto para std::shared_ptr
#include "Mesh.h" // Asegúrate de incluir la definición de Mesh


class Component_Mesh : public Component {
public:
    // Constructor que acepta un GameObject y un Mesh
    Component_Mesh(std::shared_ptr<GameObject> containerGO, std::shared_ptr<Mesh> mesh);
    virtual ~Component_Mesh() override;

    void Enable() override;
    void Disable() override;
    void Update(double dt) override;
    void DrawComponent() override;

    void LoadMesh(aiMesh* ai_mesh);
    void LoadMesh(const std::string& path);

    GLuint vao; // Vertex Array Object

private:
    std::shared_ptr<Mesh> mesh; // Almacena el puntero a la malla
};

#endif // !__COMPONENT_MESH_H__
