#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <GL/glew.h>

class Material {
public:
    Material(const std::string& name); // Declaración del constructor

    void SetDiffuseTexture(GLuint textureId);
    GLuint GetDiffuseTexture() const;

    void Apply() const;

    std::string name;
    GLuint diffuseTextureId;
    float shininess;
    GLuint specularTexture;
    GLuint normalTexture;
};

#endif // MATERIAL_H
