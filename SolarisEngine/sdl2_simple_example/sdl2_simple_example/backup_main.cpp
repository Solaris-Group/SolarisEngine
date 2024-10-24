﻿#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>
#include "imgui_impl_sdl2.h"
#include "MyWindow.h"
#include "Importer.h"
#include "CameraEditor.h"


#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <iostream>
#include <vector>
#include "Primitivos.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1024, 720);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

GLuint textureID;
const char* fbxFilePath = "./Assets/h.fbx";

const struct aiScene* scene = aiImportFile(fbxFilePath, aiProcess_Triangulate);

Importer& importer = Importer::getInstance();

    
CameraEditor camera(glm::vec3(0.0f, 0.3f, 0.2f), // Posici�n inicial
    glm::vec3(0.0f, 0.0f, -1.0f), // Direcci�n de la c�mara
    glm::vec3(0.0f, 1.0f, 0.0f)); // Vector up



std::shared_ptr<GameObject> gameObject;


static void init_openGL() {
	if (glewInit() != GLEW_OK) throw exception("Failed to initialize GLEW.");
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void initializeDevIL() {
	ilInit();
	iluInit();
}

bool loadImageWithDevIL(const char* filename) {
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadImage((const wchar_t*)filename)) {
		std::cerr << "Error loading image: " << ilGetError() << std::endl;
		ilDeleteImages(1, &imageID);
		return false;
	}

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	GLubyte* imageData = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	ilDeleteImages(1, &imageID);
	return true;
}

std::vector<GLuint> VAOs;
std::vector<GLuint> VBOs;

static void DrawFBX(const struct aiScene* scene) {
	if (!scene) {
		fprintf(stderr, "Error loading the file: %s\n", aiGetErrorString());
		return;
	}

	VAOs.resize(scene->mNumMeshes);
	VBOs.resize(scene->mNumMeshes);

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];

		glGenVertexArrays(1, &VAOs[i]);
		glGenBuffers(1, &VBOs[i]);

		glBindVertexArray(VAOs[i]);

		// Load vertices
		std::vector<GLfloat> vertices(mesh->mNumVertices * 3);
		for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
			aiVector3D vertex = mesh->mVertices[v];
			vertices[v * 3] = vertex.x;
			vertices[v * 3 + 1] = vertex.y;
			vertices[v * 3 + 2] = vertex.z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

static void display_func(const struct aiScene* scene) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	if (loadImageWithDevIL("cat.jpg")) {
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	// Draw FBX model
	DrawFBX(scene);

	glDisable(GL_TEXTURE_2D);
}

static bool processEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {
			return false;
			break;
		}
		case SDL_KEYDOWN: {
			camera.processInput(event.key.keysym.sym); // Procesa entradas de la cámara
			break;
		}
		case SDL_MOUSEMOTION: {
			// Obtiene el desplazamiento del ratón
			 // Procesa el movimiento del ratón
	
			float xoffset = event.motion.xrel; 
			float yoffset = event.motion.yrel;

			if (event.button.button == SDL_BUTTON_MIDDLE) {
				camera.processMouseMovement(xoffset, yoffset);
			}

			break;
		}
		case SDL_MOUSEWHEEL: {
			// Procesa el desplazamiento de la rueda del ratón
			if (event.wheel.y > 0) {
				camera.MouseWheel(true);
			}
			else if (event.wheel.y < 0) {
				camera.MouseWheel(false);
			}
			break;
		}
		default:
			ImGui_ImplSDL2_ProcessEvent(&event);
			break;
		}

	}
	return true;
}


static bool Start() {

    //importer.Importar(fbxFilePath);
	gameObject = importer.Importar(fbxFilePath);


	return true;
}


static bool PreUpdate() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}



static bool Update() {

	camera.Update();


	CubeImmediateMode cubo;
	cubo.draw();


    //importer.Draw("PDAFT Miku_mesh");

    //DrawFBX(scene);

	

	if (gameObject) {
		// En tu ciclo de renderizado
		gameObject->Draw(); // Llama al método Draw del GameObject importado
	}



    /*const float radius = 4.2f;
    float camX = sin(0.2f) * radius;
    float camZ = cos(0.5f) * radius;
    glm::mat4 view;



	view = lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/


	return true;

}



int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
	Importer& importer = Importer::getInstance();

	// Import the model
	//const struct aiScene* scene = importer.Importar(fbxFilePath);

	init_openGL();

	Start();


	while (processEvents()) {
		const auto t0 = hrclock::now();

		//display_func(scene);

		//Preupdate
		PreUpdate();

		//Update //Dibujar
		Update();



		window.swapBuffers();

		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;

		if (dt < FRAME_DT) {
			std::this_thread::sleep_for(FRAME_DT - dt);
		}
	}

	// Cleanup
	for (unsigned int i = 0; i < VAOs.size(); i++) {
		glDeleteVertexArrays(1, &VAOs[i]);
		glDeleteBuffers(1, &VBOs[i]);
	}

	return 0;
}





