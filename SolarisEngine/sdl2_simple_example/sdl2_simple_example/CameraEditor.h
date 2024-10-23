#ifndef CAMERA_EDITOR_H
#define CAMERA_EDITOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using vec3 = glm::dvec3;

class CameraEditor {
public:
    glm::mat4x4 lookAt(vec3 const& eye, vec3 const& center, vec3 const& up);
    CameraEditor(glm::vec3 position, glm::vec3 front, glm::vec3 up);

    glm::mat4 getViewMatrix() const;
    void processInput(unsigned char key);
    void processMouseMovement(float xoffset, float yoffset);
    void updatePosition(glm::vec3 delta);
    void Update();

    void MouseWheel(bool zoom);

private:
    glm::vec3 position; // Posici�n de la c�mara
    glm::vec3 front;    // Direcci�n hacia donde apunta la c�mara
    glm::vec3 up;       // Vector hacia arriba de la c�mara

    float yaw;          // Rotaci�n alrededor del eje Y
    float pitch;        // Rotaci�n alrededor del eje X
    const float sensitivity = 0.1f; // Sensibilidad del movimiento del rat�n
};

#endif // CAMERA_EDITOR_H