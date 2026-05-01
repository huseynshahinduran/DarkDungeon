#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <vector>

#include "Display.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "transform.h"

//Tracks the current state of the application
enum class GameState { PLAY, EXIT };

//The core enigne class managing the game loop, rendering, and logic
class MainGame
{
public:
	MainGame();
	~MainGame();

	//Starts the main application loop
	void run();

private:
	//Core lifecycle methods
	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();

	//Shader configuration helpers
	void linkADS(const Transform& t, const glm::vec3& objectColor, bool useTexture, bool useRim, const glm::vec3& rimColor, float rimPower);
	void linkToon(const Transform& t, const glm::vec3& objectColor, bool useTexture);
	
	//Mesh generation and rendering helpers
	void resetScene();
	void createWallMesh(Mesh& mesh, float width, float height);
	void drawObjectADS(Mesh& mesh, Texture& tex, const Transform& t, const glm::vec3& color, bool useTex, bool rim, const glm::vec3& rimCol, float rimPow);
	void drawObjectToon(Mesh& mesh, Texture& tex, const Transform& t, const glm::vec3& color, bool useTex);
	void drawFlame(Transform& t, float timeOffset);

	//Physics/Logic
	bool checkCollision(const glm::vec3& pos, const glm::vec3& boxMin, const glm::vec3& boxMax);

	Display _gameDisplay;
	GameState _gameState;

	//Shaders for different visual effects
	Shader passthrough;
	Shader ADS; //Ambient, Diffuse, Specular lighting
	Shader toonShader; //Cel-shaded look
	Shader flameShader;

	//3D Geometry (Mesh)
	Mesh meshTorch;
	Mesh meshChest;      
	Mesh meshDoor;
	Mesh meshKey;
	Mesh meshBarrel;
	Mesh meshFlame;
	Mesh meshTable;

	//Surface Images (Textures)
	Texture textureTorch;
	Texture textureChest;
	Texture textureDoor;
	Texture textureKey;
	Texture textureBarrel;
	Texture textureTable;
	
	//=== Room 1 (Starter) ===
	//Walls
	Mesh meshWall;
	Mesh meshWallHalfL;
	Mesh meshWallHalfR;
	Transform wallRoom1Back;
	Transform wallRoom1Left;
	Transform wallRoom1Right;
	Transform wallRoom1Front;
	Transform wallRoom1FrontL;
	Transform wallRoom1FrontR;
	Transform wallRoom1Floor;
	Transform wallRoom1Ceiling;
	Mesh meshFloorCeil;

	//Barrel + Key
	Transform transformBarrelKey;
	Transform transformKey;

	//Wall torches Room 1
	Transform wallTorch1_1;
	Transform wallTorch1_2;
	Transform wallTorch1_3;
	Transform wallTorch1_4;

	//=== Room 2 (Main) Transforms ===
	Transform wallRoom2Back;
	Transform wallRoom2Left;
	Transform wallRoom2Right;
	Transform wallRoom2Floor;
	Transform wallRoom2Ceiling;

	//Chest + Table
	Transform transformChest;
	Transform transformTable;

	//Barrels Room 2
	Transform transformBarrel2_1;
	Transform transformBarrel2_2;
	Transform transformBarrel2_3;
	Transform transformBarrel2_4;

	//Wall torches Room 2
	Transform wallTorch2_1;
	Transform wallTorch2_2;
	Transform wallTorch2_3;
	Transform wallTorch2_4;

	//Door
	Transform transformDoor;

	//Player torch
	Transform transformTorch;
	Transform transformFlamePlayer;

	//Viewport/Camera
	Camera myCamera;

	//Global Lighting
	glm::vec3 lightPos;

	//Animation and timing variables
	float counter;
	float chestRotation;
	bool autoRotate;

	//Mouse look tracking
	bool firstMouse;
	float lastMouseX;
	float lastMouseY;
	float mouseSensitivity;

	//Frame timing calculate
	Uint32 lastFrameTime;
	float deltaTime;

	//Visual toggles
	bool useToonMode;

	//Gameplay state flags
	bool hasKey;
	bool doorOpen;
};