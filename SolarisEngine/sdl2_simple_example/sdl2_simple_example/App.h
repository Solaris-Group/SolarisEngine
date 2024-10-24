#include <string>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/vector_int2.hpp>
#include <exception>
#include <thread>


#include "Primitivos.h"


#include "MyWindow.h"
#include "Importer.h"
#include "CameraEditor.h"



#ifndef __APP_H__
#define __APP_H__




// Classes "globales"
class MyWindow;
class Importer;
class CameraEditor;


class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	//void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;


	// Request a save data in an XML file 
	bool LoadRequest();

	// Request to load data from XML file 
	bool SaveRequest();

	bool IsSaving();

private:

	// Load config file
	bool LoadConfig();

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();
	bool PostLateUpdate();

	// Reads XML file and loads the data
	bool LoadFromFile();

	// Sace XML file with modules data
	bool SaveFromFile();


	bool INIT_openGL();

	bool processEvents();



public:

	float dt;

	Importer* importer;
	CameraEditor* cameraEditor;
	MyWindow* myWindow;

	static inline const glm::ivec2 WINDOW_SIZE = { 1024, 720 };
	static const unsigned int FPS = 60;
	static inline const auto FRAME_DT = 1.0s / FPS;


private:

	int argc;
	char** args;
	std::string gameTitle;
	std::string organization;

	std::shared_ptr<GameObject> importedObject;

	//List<Module *> modules;	
	
	//uint frames;
	

	/*
	Timer startupTime;
	PerfTimer frameTime;
	PerfTimer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	uint32 secondsSinceStartup = 0;*/

	//L02 DONE 1: Set the maximun frame duration in miliseconds.
	//uint32 maxFrameDuration = 16;

	//
	bool loadRequest = false;
	bool saveRequest = false;
	bool isSaving = false;

};

extern App* app;

#endif	// __APP_H__