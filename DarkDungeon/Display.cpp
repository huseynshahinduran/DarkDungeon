#include "Display.h"

Display::Display()
{
	screenWidth = 1024.0f; //Set default window width
	screenHeight = 768.0f; //Set default window height
	sdlWindow = nullptr;
}

Display::~Display()
{
	//Clean up OpenGL and SDL resources when the window is closed
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(sdlWindow);
	SDL_Quit();
}

void Display::initDisplay()
{
	//Initialize all SDL subsystems (video, audio, etc.)
	SDL_Init(SDL_INIT_EVERYTHING);

	//Configure OpenGL to use double buffering to prevent screen tearing
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Enable VSync (sync frame rendering with monitor refresh rate)
	SDL_GL_SetSwapInterval(1);

	//Create the application window centered on the screen
	sdlWindow = SDL_CreateWindow(
		"Dark Dungeon",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		(int)screenWidth,
		(int)screenHeight,
		SDL_WINDOW_OPENGL
	);

	if (sdlWindow == nullptr)
		returnError("SDL Window could not be created!");

	//Create the OpenGL rendering context attached to our SDL window
	glContext = SDL_GL_CreateContext(sdlWindow);

	if (glContext == nullptr)
		returnError("SDL_GL context could not be created!");

	//Initialize GLEW to load modern OpenGL functions
	GLenum error = glewInit();

	if (error != GLEW_OK)
		returnError("GLEW could not be initialized!");

	//Enable essential 3D rendering features
	glEnable(GL_DEPTH_TEST); //Ensures objects closer to the camera obscure farther ones
	glEnable(GL_CULL_FACE); //Optimizes rendering by skipping the back sides of geometry

	cout << "Display successfully initialised!" << endl;
}

void Display::swapBuffer()
{
	///Swap the hidden drawing buffer with the visible screen buffer to show the new frame
	SDL_GL_SwapWindow(sdlWindow);
}

void Display::clearDisplay(float r, float g, float b, float a)
{
	//Set the background color and wipe out the old frame's color and depth data
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float Display::getWidth()
{
	return screenWidth;
}

float Display::getHeight()
{
	return screenHeight;
}

void Display::returnError(string errorString)
{
	//Print the fatal error, pause for user acknowledgment, and exit safely
	cout << errorString << endl;
	cout << "Press any key to quit..." << endl;
	int in;
	cin >> in;
	SDL_Quit();
}