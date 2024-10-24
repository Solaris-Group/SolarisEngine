#include <GL/glew.h> // Asegúrate de incluir GLEW antes de OpenGL
#include <GL/gl.h>

#include "App.h"
#include "Defs.h"
#include <iostream>
#include <exception>
#include <imgui_impl_sdl2.h>
#include <SDL2/SDL_events.h>
#include "Primitivos.h"


App* app = NULL;

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args), loadRequest(false), saveRequest(false), isSaving(false)
{
    // Inicializar el título del juego y la organización
    gameTitle = "My Game";
    organization = "My Organization";


    srand(static_cast<unsigned int>(time(0)));

}

// Destructor
App::~App()
{
    // Llamar a CleanUp antes de destruir la aplicación
    CleanUp();
}

// Called before render is available
bool App::Awake()
{
    std::cout << "App::Awake called" << std::endl;
    // Aquí podrías cargar la configuración del juego
    return LoadConfig();
}

// Called before the first frame
bool App::Start()
{
    std::cout << "App::Start called" << std::endl;
    // Iniciar las cosas necesarias antes del primer frame

    



    return true;
}

// Called each loop iteration
bool App::Update()
{
    std::cout << "App::Update called" << std::endl;

    PrepareUpdate();


    while (processEvents()) {
        const auto t0 = hrclock::now();

        if (!PreUpdate()) return false;
        if (!DoUpdate()) return false;
        if (!PostUpdate()) return false;
        if (!PostLateUpdate()) return false;

        myWindow->swapBuffers();

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;

        if (dt < FRAME_DT) {
            std::this_thread::sleep_for(FRAME_DT - dt);
        }
    }


    FinishUpdate();

    return false;
}

// Called before quitting
bool App::CleanUp()
{
    std::cout << "App::CleanUp called" << std::endl;
    // Limpiar y liberar recursos antes de salir
    return true;
}

// Getters
int App::GetArgc() const
{
    return argc;
}

const char* App::GetArgv(int index) const
{
    if (index < argc) {
        return args[index];
    }
    return nullptr;
}

const char* App::GetTitle() const
{
    return gameTitle.c_str();
}

const char* App::GetOrganization() const
{
    return organization.c_str();
}

// Requests
bool App::LoadRequest()
{
    loadRequest = true;
    return loadRequest;
}

bool App::SaveRequest()
{
    saveRequest = true;
    return saveRequest;
}

bool App::IsSaving()
{
    return isSaving;
}

// Private methods

bool App::LoadConfig()
{
    myWindow = new MyWindow("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    importer = &Importer::getInstance();
    cameraEditor = new CameraEditor(
        glm::vec3(0.0f, 0.3f, 0.2f),    // Posici�n inicial
        glm::vec3(0.0f, 0.0f, -1.0f),   // Dirección de la c�mara
        glm::vec3(0.0f, 1.0f, 0.0f));   // Vector up


    INIT_openGL();

    importedObject = importer->Importar("./Assets/c.fbx");



    return true;
}

void App::PrepareUpdate()
{
    // Preparar la lógica antes de actualizar cada módulo
    //std::cout << "App::PrepareUpdate called" << std::endl;
}

void App::FinishUpdate()
{
    // Finalizar la lógica después de actualizar cada módulo
    //std::cout << "App::FinishUpdate called" << std::endl;
}

bool App::PreUpdate()
{
    // Llamar a la lógica de pre-actualización de los módulos
    //std::cout << "App::PreUpdate called" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
}

bool App::DoUpdate()
{
    // Llamar a la lógica de actualización principal de los módulos
    //std::cout << "App::DoUpdate called" << std::endl;

    cameraEditor->Update();
    CubeImmediateMode cube;
    cube.draw();

    if (importedObject) {
        importedObject->Draw();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
    }
    else {
        std::cerr << "El objeto importado es nulo." << std::endl;
    }

    return true;
}

bool App::PostUpdate()
{
    // Llamar a la lógica de post-actualización de los módulos
    //std::cout << "App::PostUpdate called" << std::endl;
    return true;
}

bool App::PostLateUpdate()
{
    // Llamar a la lógica de post-actualización final de los módulos
    //std::cout << "App::PostLateUpdate called" << std::endl;
    return true;
}

bool App::LoadFromFile()
{
    //std::cout << "App::LoadFromFile called" << std::endl;
    // Lógica para cargar datos desde un archivo
    return true;
}

bool App::SaveFromFile()
{
    //std::cout << "App::SaveFromFile called" << std::endl;
    // Lógica para guardar datos en un archivo
    return true;
}

bool App::INIT_openGL() {
    // Inicializar GLEW después de crear el contexto de OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error inicializando GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    // Asegúrate de que la versión de OpenGL es la que necesitas
    if (!GLEW_VERSION_3_0) {
        std::cerr << "OpenGL 3.0 no está disponible." << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    return true;
}

bool App::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            return false;
            break;
        }
        case SDL_KEYDOWN: {
            cameraEditor->processInput(event.key.keysym.sym); // Procesa entradas de la cámara
            break;
        }
        case SDL_MOUSEMOTION: {
            // Obtiene el desplazamiento del ratón
             // Procesa el movimiento del ratón

            float xoffset = event.motion.xrel;
            float yoffset = event.motion.yrel;

            if (event.button.button == SDL_BUTTON_MIDDLE) {
                cameraEditor->processMouseMovement(xoffset, yoffset);
            }

            break;
        }
        case SDL_MOUSEWHEEL: {
            // Procesa el desplazamiento de la rueda del ratón
            if (event.wheel.y > 0) {
                cameraEditor->MouseWheel(true);
            }
            else if (event.wheel.y < 0) {
                cameraEditor->MouseWheel(false);
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


