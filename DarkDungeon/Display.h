#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>

using namespace std;

//Manages the applications window and OpenGL rendering context
class Display
{
public:
	Display();
	~Display();

	//Creates the window and initializes OpenGL
	void initDisplay();
	
	//Updates the screen with the newly rendered frame
	void swapBuffer();

	//Clears the screen using a specific RGBA background color
	void clearDisplay(float r, float g, float b, float a);

	//Window dimension getters
	float getWidth();
	float getHeight();

private:
	//Utility to print error messages and handle failures
	void returnError(string errorString);

	SDL_GLContext glContext; //OpenGL state machine linked to the window
	SDL_Window* sdlWindow; //Pointer to the SDL window instance
	float screenWidth;
	float screenHeight;
};