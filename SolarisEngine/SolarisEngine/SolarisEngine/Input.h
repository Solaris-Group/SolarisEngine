#ifndef __INPUT_EDITOR_H__
#define __INPUT_EDITOR_H__

#define MAX_KEYS 256
#define MAX_PADS 4

#include "App.h"
#include <imgui_impl_sdl2.h>

struct _SDL_GameController;
struct _SDL_Haptic;

enum Key_State
{
	KEY_IDLE,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct GamePad
{
	//Input data
	bool start, back, guide;
	bool start_down, back_down, guide_down;
	bool _start_down, _back_down, _guide_down;
	bool x, y, a, b, l1, r1, l3, r3;
	bool _x_down, _y_down, _a_down, _b_down, _l1_down, _r1_down, _l3_down, _r3_down;
	bool x_down, y_down, a_down, b_down, l1_down, r1_down, l3_down, r3_down;
	bool up, down, left, right;
	bool _up_down, _down_down, _left_down, _right_down;
	bool up_down, down_down, left_down, right_down;
	float l2, r2;
	float l_x, l_y, r_x, r_y, l_dz, r_dz;

	//Controller data
	bool enabled;
	int index;
	_SDL_GameController* controller;
	_SDL_Haptic* haptic;

	//Rumble controller
	int rumble_countdown;
	float rumble_strength;





};

class InputEditor
{
public:
	// Constructor
	InputEditor();

	// Destructor
	~InputEditor();

	// Called on application start.
	// Initializes the SDL system for input detection
	bool Init() ;

	// Called on application exit.
	// Uninitializes the SDL system for input detection
	bool CleanUp() ;

	// Activates SDL device funcionallity when a gamepad has been connected
	void HandleDeviceConnection(int index);

	// Deactivates SDL device funcionallity when a gamepad has been disconnected
	void HandleDeviceRemoval(int index);

	bool processEvents(const SDL_Event& event);

	void handleDroppedFile(const char* filePath);

	void CopyToAssetsFolder(const std::string& filePath, const std::string& destinationFolder);

	// Called at PreUpdate
	// Iterates through all active gamepads and update all input data
	void UpdateGamepadsInput();

	bool ShakeController(int id, int duration, float strength = 0.5f);
	const char* GetControllerName(int id) const;

public:
	// An array to fill in the state of all the keyboard keys
	Key_State keys[MAX_KEYS] = { KEY_IDLE };

	// An array to fill in all detected gamepads
	GamePad pads[MAX_PADS];

	bool mouseLeftIsPressed = false;
	bool mouseRightIsPressed = false;
	bool mouseMiddleIsPressed = false;

	float xoffset;
	float yoffset;

	bool isCameraMoving = false;
};

#endif // __ModuleInput_H__



