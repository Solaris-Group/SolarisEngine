
#include "Input.h"
#include "MyWindow.h"
#include <SDL2/SDL.h>
#include "Debug.h"


InputEditor::InputEditor() 
{
	for (uint i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	memset(&pads[0], 0, sizeof(GamePad) * MAX_PADS);
}

InputEditor::~InputEditor()
{

}

bool InputEditor::Init()
{
	//LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		//LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
	{
		//LOG("SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
	{
		//LOG("SDL_INIT_HAPTIC could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool InputEditor::CleanUp()
{
	//LOG("Quitting SDL input event subsystem");

	// Stop rumble from all gamepads and deactivate SDL functionallity
	for (uint i = 0; i < MAX_PADS; ++i)
	{
		if (pads[i].haptic != nullptr)
		{
			SDL_HapticStopAll(pads[i].haptic);
			SDL_HapticClose(pads[i].haptic);
		}
		if (pads[i].controller != nullptr) SDL_GameControllerClose(pads[i].controller);
	}

	SDL_QuitSubSystem(SDL_INIT_HAPTIC);
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}

void InputEditor::HandleDeviceConnection(int index)
{
	if (SDL_IsGameController(index))
	{
		for (int i = 0; i < MAX_PADS; ++i)
		{
			GamePad& pad = pads[i];

			if (pad.enabled == false)
			{
				if (pad.controller = SDL_GameControllerOpen(index))
				{
					//LOG("Found a gamepad with id %i named %s", i, SDL_GameControllerName(pad.controller));
					pad.enabled = true;
					pad.l_dz = pad.r_dz = 0.1f;
					pad.haptic = SDL_HapticOpen(index);
					if (pad.haptic != nullptr)
						//LOG("... gamepad has force feedback capabilities");
					pad.index = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad.controller));
				}
			}
		}
	}
}

void InputEditor::HandleDeviceRemoval(int index)
{
	// If an existing gamepad has the given index, deactivate all SDL device functionallity
	for (int i = 0; i < MAX_PADS; ++i)
	{
		GamePad& pad = pads[i];
		if (pad.enabled && pad.index == index)
		{
			SDL_HapticClose(pad.haptic);
			SDL_GameControllerClose(pad.controller);
			memset(&pad, 0, sizeof(GamePad));
		}
	}
}

void InputEditor::UpdateGamepadsInput()
{
	// Iterate through all active gamepads and update all input data
	for (int i = 0; i < MAX_PADS; ++i)
	{
		GamePad& pad = pads[i];

		if (pad.enabled == true)
		{
			pad._a_down = pad.a;
			pad._b_down = pad.b;
			pad._x_down = pad.x;
			pad._y_down = pad.y;
			pad._l1_down = pad.l1;
			pad._r1_down = pad.r1;
			pad._l3_down = pad.l3;
			pad._r3_down = pad.r3;
			pad._up_down = pad.up;
			pad._down_down = pad.down;
			pad._left_down = pad.left;
			pad._right_down = pad.right;
			pad._start_down = pad.start;
			pad._back_down = pad.back;
			pad._guide_down = pad.guide;


			pad.a = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_A) == 1;
			pad.b = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_B) == 1;
			pad.x = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_X) == 1;
			pad.y = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_Y) == 1;
			pad.l1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1;
			pad.r1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1;
			pad.l3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1;
			pad.r3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1;
			pad.up = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1;
			pad.down = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1;
			pad.left = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1;
			pad.right = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1;

			pad.start = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_START) == 1;
			pad.guide = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_GUIDE) == 1;
			pad.back = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_BACK) == 1;

			pad.l2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / 32767.0f;
			pad.r2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / 32767.0f;

			pad.l_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTX)) / 32767.0f;
			pad.l_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTY)) / 32767.0f;
			pad.r_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTX)) / 32767.0f;
			pad.r_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTY)) / 32767.0f;

			// Apply deadzone. All values below the deadzone will be discarded
			pad.l_x = (fabsf(pad.l_x) > pad.l_dz) ? pad.l_x : 0.0f;
			pad.l_y = (fabsf(pad.l_y) > pad.l_dz) ? pad.l_y : 0.0f;
			pad.r_x = (fabsf(pad.r_x) > pad.r_dz) ? pad.r_x : 0.0f;
			pad.r_y = (fabsf(pad.r_y) > pad.r_dz) ? pad.r_y : 0.0f;


			pad.a_down = (!pad._a_down && pad.a);
			pad.b_down = (!pad._b_down && pad.b);
			pad.x_down = (!pad._x_down && pad.x);
			pad.y_down = (!pad._y_down && pad.y);
			pad.l1_down = (!pad._l1_down && pad.l1);
			pad.r1_down = (!pad._r1_down && pad.r1);
			pad.l3_down = (!pad._l3_down && pad.l3);
			pad.r3_down = (!pad._r3_down && pad.r3);
			pad.up_down = (!pad._up_down && pad.up);
			pad.down_down = (!pad._down_down && pad.down);
			pad.left_down = (!pad._left_down && pad.left);
			pad.right_down = (!pad._right_down && pad.right);
			pad.start_down = (!pad._start_down && pad.start);
			pad.back_down = (!pad._back_down && pad.back);
			pad.guide_down = (!pad._guide_down && pad.guide);



			if (pad.rumble_countdown > 0)
				pad.rumble_countdown--;
		}
	}
}

bool InputEditor::ShakeController(int id, int duration, float strength)
{
	bool ret = false;

	// Check if the given id is valid within the array bounds
	if (id < 0 || id >= MAX_PADS) return ret;

	// Check if the gamepad is active and allows rumble
	GamePad& pad = pads[id];
	if (!pad.enabled || pad.haptic == nullptr || SDL_HapticRumbleSupported(pad.haptic) != SDL_TRUE) return ret;

	// If the pad is already in rumble state and the new strength is below the current value, ignore this call
	if (duration < pad.rumble_countdown && strength < pad.rumble_strength)
		return ret;

	if (SDL_HapticRumbleInit(pad.haptic) == -1)
	{
		//LOG("Cannot init rumble for controller number %d", id);
	}
	else
	{
		SDL_HapticRumbleStop(pad.haptic);
		SDL_HapticRumblePlay(pad.haptic, strength, duration / 60 * 1000); //Conversion from frames to ms at 60FPS

		pad.rumble_countdown = duration;
		pad.rumble_strength = strength;

		ret = true;
	}

	return ret;
}

const char* InputEditor::GetControllerName(int id) const
{
	// Check if the given id has a valid controller
	if (id >= 0 && id < MAX_PADS && pads[id].enabled == true && pads[id].controller != nullptr)
		return SDL_GameControllerName(pads[id].controller);

	return "unplugged";
}



bool InputEditor::processEvents(const SDL_Event& event) {
	switch (event.type) {
	case SDL_QUIT:
		return false;

	case SDL_KEYDOWN:
		app->cameraEditor->processInput(event.key.keysym.sym, true);
		break;

	case SDL_KEYUP:
		app->cameraEditor->processInput(event.key.keysym.sym, false);
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) {
			mouseLeftIsPressed = true;
		}
		if (event.button.button == SDL_BUTTON_RIGHT) {
			mouseRightIsPressed = true;
		}
		if (event.button.button == SDL_BUTTON_MIDDLE) {
			mouseMiddleIsPressed = true;
		}
		break;

	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT) {
			mouseLeftIsPressed = false;
		}
		if (event.button.button == SDL_BUTTON_RIGHT) {
			mouseRightIsPressed = false;
		}
		if (event.button.button == SDL_BUTTON_MIDDLE) {
			mouseMiddleIsPressed = false;
		}
		break;

	case SDL_MOUSEMOTION:
		// Procesar �rbita solo si Alt + clic izquierdo est�n activos
		if (mouseLeftIsPressed && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LALT]) {
			float xoffset = event.motion.xrel;
			float yoffset = event.motion.yrel;
			app->cameraEditor->processMouseMovement(xoffset, yoffset);
		}
		// Procesar movimiento de c�mara libre si clic derecho est� activo
		else if (mouseRightIsPressed) {
			float xoffset = event.motion.xrel;
			float yoffset = event.motion.yrel;
			app->cameraEditor->processMouseMovement(xoffset, yoffset);
		}
		// Procesar desplazamiento si el clic del medio est� activo
		else if (mouseMiddleIsPressed) {
			float xoffset = event.motion.xrel;
			float yoffset = event.motion.yrel;
			app->cameraEditor->processMouseMiddle(xoffset, yoffset);
		}
		break;

	case SDL_MOUSEWHEEL:
		app->cameraEditor->MouseWheel(event.wheel.y > 0);
		break;

	case SDL_DROPFILE:
		handleDroppedFile(event.drop.file);
		break;

	default:
		//ImGui_ImplSDL2_ProcessEvent(&event);
		break;
	}

	ImGui_ImplSDL2_ProcessEvent(&event);

	return true;
}



void InputEditor::handleDroppedFile(const char* filePath)
{
	std::string droppedFile = filePath;
	std::string extension = droppedFile.substr(droppedFile.find_last_of('.') + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == "fbx" || extension == "obj") {
		GameObject* gameObject = app->importer->Importar(droppedFile);
		app->actualScene->AddGameObject(gameObject);
		
	}
	else if (app->actualScene->GetSelectedGameObject() && (extension == "png" || extension == "jpg" || extension == "dds")) {
		Component_Material* cm = app->actualScene->GetSelectedGameObject()->GetComponent<Component_Material>();
		if (cm == nullptr) {
			app->actualScene->GetSelectedGameObject()->AddComponent<Component_Material>()->SetTexture(droppedFile);
		}
		else {
			app->actualScene->GetSelectedGameObject()->GetComponent<Component_Material>()->SetTexture(droppedFile);
		}
		
	}
	Debug::Log("Import file: ", filePath);
}