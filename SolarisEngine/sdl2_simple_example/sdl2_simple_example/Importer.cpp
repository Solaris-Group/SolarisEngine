#include "Importer.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Importer::Importer() {
    std::cout << "Inicializando directorios..." << std::endl;

    // Crear directorios si no existen
    for (const auto& dir : { ASSETS_DIR, LIBRARY_DIR, MESHES_DIR, MATERIALS_DIR, MODELS_DIR }) {
        if (!(fs::exists(dir) && fs::is_directory(dir))) {
            fs::create_directory(dir);
            std::cout << "Creado directorio: " << dir << std::endl;
        }
    }

    ilInit(); // Inicializa DevIL
}

std::shared_ptr<GameObject> Importer::Importar(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    if (!scene) {
        std::cerr << "Error al cargar el archivo: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Archivo cargado exitosamente: " << filepath << std::endl;

    LoadMaterials(scene);

    auto gameObject = std::make_shared<GameObject>("ImportedFBX");

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* ai_mesh = scene->mMeshes[i];

        // Crear un nuevo objeto de malla
        auto mesh = std::make_shared<Mesh>();
        mesh->LoadFromAssimp(ai_mesh); // Implementa esta función en la clase Mesh

        // Crear y agregar un componente de malla al GameObject
        auto meshComponent = std::make_unique<Component_Mesh>(gameObject, mesh);
        gameObject->AddComponent<Component_Mesh>(std::move(meshComponent));

        std::cout << "Malla procesada: " << ai_mesh->mName.C_Str() << " con " << ai_mesh->mNumVertices << " vértices." << std::endl;
    }

    return gameObject; // Devuelve el GameObject creado
}

void Importer::LoadMaterials(const aiScene* scene) {
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* ai_material = scene->mMaterials[i];
        aiString name;
        ai_material->Get(AI_MATKEY_NAME, name);
        std::cout << "Cargando material: " << name.C_Str() << std::endl;

        // Crear el material, asegurando de pasar el nombre
        auto material = std::make_shared<Material>(name.C_Str()); // Aquí se pasa el nombre

        // Cargar textura difusa si existe
        if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString texturePath;
            if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                std::string fullPath = MATERIALS_DIR + std::string("/") + texturePath.C_Str();
                GLuint textureId = LoadTexture(fullPath); // Cargar textura y obtener el ID
                material->SetDiffuseTexture(textureId); // Establecer la textura difusa
            }
        }

        materials[name.C_Str()] = material; // Guardar el material en el mapa
        std::cout << "Material cargado: " << material->name << std::endl;
    }
}


GLuint Importer::LoadTexture(const std::string& fullPath) {
    GLuint textureId;
    ilGenImages(1, &textureId);
    ilBindImage(textureId);

    if (ilLoadImage((const wchar_t*)fullPath.c_str())) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Textura cargada: " << fullPath << std::endl;
    }
    else {
        std::cerr << "Error al cargar la textura: " << fullPath << std::endl;
    }

    ilDeleteImages(1, &textureId); // Limpiar ID de textura
    return textureId;
}

void Importer::Draw(const std::shared_ptr<GameObject>& gameObject) {
    if (gameObject) {
        gameObject->Draw(); // Llama al método Draw del GameObject
    }
    else {
        std::cerr << "El GameObject es nulo, no se puede dibujar." << std::endl;
    }
}



//#include "Importer.h"
//#include "GameObject.h"
//#include "Component_Mesh.h" // Asegúrate de que tienes esta clase definida
//#include <memory> // Para std::shared_ptr
//
//namespace fs = std::filesystem;
//
//Importer::Importer() {
//    std::cout << "Inicializando directorios..." << std::endl;
//
//    // Crear directorios si no existen
//    for (const auto& dir : { ASSETS_DIR, LIBRARY_DIR, MESHES_DIR, MATERIALS_DIR, MODELS_DIR }) {
//        if (!(fs::exists(dir) && fs::is_directory(dir))) {
//            fs::create_directory(dir);
//            std::cout << "Creado directorio: " << dir << std::endl;
//        }
//    }
//
//    ilInit(); // Inicializa DevIL
//}
//
//std::shared_ptr<GameObject> Importer::Importar(const std::string& filepath) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
//
//    if (!scene) {
//        std::cerr << "Error al cargar el archivo: " << importer.GetErrorString() << std::endl;
//        return nullptr;
//    }
//
//    std::cout << "Archivo cargado exitosamente: " << filepath << std::endl;
//
//    LoadMaterials(scene);
//
//    auto gameObject = std::make_shared<GameObject>("ImportedFBX");
//
//    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
//        aiMesh* ai_mesh = scene->mMeshes[i];
//        std::string meshName = ai_mesh->mName.C_Str();
//        meshes.push_back(meshName);
//
//        // Crear y agregar un componente de malla al GameObject
//        auto meshComponent = std::make_unique<Component_Mesh>(gameObject);
//        meshComponent->LoadMesh(ai_mesh); // Implementa esta función en Component_Mesh
//        gameObject->AddComponent<Component_Mesh>(std::move(meshComponent));
//
//        std::cout << "Malla procesada: " << meshName << " con " << ai_mesh->mNumVertices << " vértices." << std::endl;
//    }
//
//    return gameObject; // Devuelve el GameObject creado
//}
//
//void Importer::LoadMaterials(const aiScene* scene) {
//    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
//        aiMaterial* ai_material = scene->mMaterials[i];
//        aiString name;
//        ai_material->Get(AI_MATKEY_NAME, name);
//        std::cout << "Name: " << name.C_Str() << std::endl;
//        materials.push_back(name.C_Str());
//
//        // Cargar textura difusa si existe
//        if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
//            aiString texturePath;
//            if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
//                std::string fullPath = MATERIALS_DIR + std::string("/") + texturePath.C_Str();
//                GLuint textureId;
//                ilGenImages(1, &textureId);
//                ilBindImage(textureId);
//
//                if (ilLoadImage((const wchar_t*)fullPath.c_str())) {
//                    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
//                    glGenTextures(1, &textureId);
//                    glBindTexture(GL_TEXTURE_2D, textureId);
//                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
//                    glGenerateMipmap(GL_TEXTURE_2D);
//
//                    textureIds[name.C_Str()] = textureId; // Guardar ID de textura
//                    std::cout << "Textura cargada: " << fullPath << std::endl;
//                }
//                else {
//                    std::cerr << "Error al cargar la textura: " << fullPath << std::endl;
//                }
//
//                ilDeleteImages(1, &textureId); // Limpiar ID de textura
//            }
//        }
//    }
//}
//
//void Importer::Draw(const std::shared_ptr<GameObject>& gameObject) {
//    if (gameObject) {
//        gameObject->Draw(); // Llama al método Draw del GameObject
//    }
//    else {
//        std::cerr << "El GameObject es nulo, no se puede dibujar." << std::endl;
//    }
//}
//
//void Importer::Draw(const std::string& modelName) {
//    auto it = vaos.find(modelName);
//    if (it == vaos.end()) {
//        std::cerr << "Modelo no encontrado: " << modelName << std::endl;
//        return;
//    }
//
//    GLuint vao = it->second; // Obtener el VAO
//    glBindVertexArray(vao);
//
//    // Aquí asumiendo que tienes un método para obtener el número de índices
//    // glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
//
//    std::cout << "Dibujando modelo: " << modelName << std::endl;
//}
