#include "Material.h"
#include <iostream>

// Constructor
Material::Material(const std::string& name)
    : name(name), diffuseTextureId(0), shininess(32.0f), specularTexture(0), normalTexture(0) { // Inicializa valores por defecto
}

// Establecer la textura difusa
void Material::SetDiffuseTexture(GLuint textureId) {
    diffuseTextureId = textureId;
}

// Obtener la textura difusa
GLuint Material::GetDiffuseTexture() const {
    return diffuseTextureId;
}

// Aplicar el material
void Material::Apply() const {
    if (diffuseTextureId != 0) {
        glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
    }
    else {
        std::cerr << "Advertencia: No se ha configurado la textura difusa para el material: " << name << std::endl;
    }

    // Aquí puedes establecer otras propiedades del material (especular, normales, etc.)
    // Ejemplo: glUniform1f(shaderProgram->getUniformLocation("material.shininess"), shininess);
}
