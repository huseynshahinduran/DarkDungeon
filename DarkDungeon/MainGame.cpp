#include "MainGame.h"
#include <iostream>

//Room Dimensions
const float ROOM_WIDTH = 30.0f;
const float ROOM_HEIGHT = 8.5f;
const float ROOM1_DEPTH_MIN = -30.0f;
const float ROOM1_DEPTH_MAX = 0.0f;
const float ROOM2_DEPTH_MIN = 0.0f;
const float ROOM2_DEPTH_MAX = 40.0f;
const float DOOR_WIDTH = 4.0f;

MainGame::MainGame() 
{
	_gameState = GameState::PLAY;
	counter = 0.0f;
	chestRotation = 0.0f;
	autoRotate = true;
	lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

	firstMouse = true;
	lastMouseX = 512.0f;
	lastMouseY = 384.0f;
	mouseSensitivity = 0.15f;

	lastFrameTime = 0;
	deltaTime = 0.0f;

	useToonMode = false;
	hasKey = false;
	doorOpen = false;
}

MainGame::~MainGame() {}

void MainGame::run()
{
	initSystems();
	gameLoop();
}

void MainGame::createWallMesh(Mesh& mesh, float width, float height)
{
	Vertex verts[] =
	{
		Vertex(glm::vec3(-width / 2.0f, -height / 2.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(width / 2.0f, -height / 2.0f, 0.0f), glm::vec2(width / 5.0f, 0.0f)),
		Vertex(glm::vec3(-width / 2.0f, height / 2.0f, 0.0f), glm::vec2(0.0f, height / 5.0f)),
		Vertex(glm::vec3(width / 2.0f, height / 2.0f, 0.0f), glm::vec2(width / 5.0f, height / 5.0f))
	};
	unsigned int indices[] = { 0, 1, 2, 2, 1, 3 };
	mesh.init(verts, 4, indices, 6);
}

void MainGame::drawObjectADS(Mesh& mesh, Texture& tex, const Transform& t, const glm::vec3& color, bool useTex, bool rim, const glm::vec3& rimCol, float rimPow)
{
	ADS.Update(t, myCamera);
	linkADS(t, color, useTex, rim, rimCol, rimPow);
	tex.Bind(0);
	mesh.draw();
}

void MainGame::drawObjectToon(Mesh& mesh, Texture& tex, const Transform& t, const glm::vec3& color, bool useTex)
{
	toonShader.Update(t, myCamera);
	linkToon(t, color, useTex);
	tex.Bind(0);
	mesh.draw();
}

void MainGame::drawFlame(Transform& t, float timeOffset)
{
	flameShader.Update(t, myCamera);
	flameShader.setFloat("time", counter + timeOffset);
	meshFlame.draw();
}

bool MainGame::checkCollision(const glm::vec3& pos, const glm::vec3& boxMin, const glm::vec3& boxMax)
{
	return 
		pos.x >= boxMin.x && pos.x <= boxMax.x &&
		pos.y >= boxMin.y && pos.y <= boxMax.y &&
		pos.z >= boxMin.z && pos.z <= boxMax.z;
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay();
	SDL_SetRelativeMouseMode(SDL_TRUE);

	myCamera.initCamera(
		glm::vec3(0.0f, 0.0f, -5.0f), 
		70.0f,
		(float)_gameDisplay.getWidth() / _gameDisplay.getHeight(), 
		0.1f, 
		1000.0f
	);

	//Shaders
	passthrough.init("res\\shader.vert", "res\\shader.frag");
	ADS.init("res\\ADS.vert", "res\\ADS.frag");
	toonShader.init("res\\toon.vert", "res\\toon.frag");
	flameShader.init("res\\flame.vert", "res\\flame.frag");

	//Models
	meshTorch.loadModel("res\\torch.obj");
	meshChest.loadModel("res\\chest.obj");
	meshDoor.loadModel("res\\door.obj");
	meshKey.loadModel("res\\key.obj");
	meshBarrel.loadModel("res\\barrel.obj");
	meshTable.loadModel("res\\table.obj");

	//Flame quad
	Vertex flameVerts[] =
	{
		Vertex(glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-0.5f, 1.5f, 0.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(0.5f, 1.5f, 0.0f), glm::vec2(1.0f, 1.0f))
	};
	unsigned int flameIndices[] = { 0, 1, 2, 2, 1, 3 };
	meshFlame.init(flameVerts, 4, flameIndices, 6);

	//Walls
	createWallMesh(meshWall, ROOM_WIDTH * 2.0f, ROOM_HEIGHT);
	createWallMesh(meshFloorCeil, ROOM_WIDTH * 2.0f, 40.0f);

	float halfWidth = (ROOM_WIDTH - DOOR_WIDTH);
	createWallMesh(meshWallHalfL, halfWidth, ROOM_HEIGHT);
	createWallMesh(meshWallHalfR, halfWidth, ROOM_HEIGHT);

	//Textures
	textureTorch.init("res\\torch_diff.png");
	textureChest.init("res\\chest_diff.png");
	textureDoor.init("res\\door_diff.png");
	textureKey.init("res\\key_diff.png");
	textureBarrel.init("res\\barrel_diff.png");
	textureTable.init("res\\table_diff.png");

	resetScene();

	std::cout << "All systems initialized!" << std::endl;
	std::cout << "\n========= DARK DUNGEON =========" << std::endl;
	std::cout << "You are locked up in a dark dungeon!" << std::endl;
	std::cout << "Find the key on the barrel," << std::endl;
	std::cout << "open the door and get to the treasure!" << std::endl;
	std::cout << "================================" << std::endl;
	std::cout << "\n=== CONTROLS ===" << std::endl;
	std::cout << "MOUSE      - Look Around" << std::endl;
	std::cout << "WASD       - Movement" << std::endl;
	std::cout << "E          - Pick up key / Open door" << std::endl;
	std::cout << "T          - Toon Shading on/off" << std::endl;
	std::cout << "R          - Reset Scene" << std::endl;
	std::cout << "ESC        - Exit" << std::endl;
	std::cout << "==================\n" << std::endl;
}

void MainGame::resetScene()
{
	//=== PLAYER ===
	myCamera.pos = glm::vec3(0.0f, 2.0f, -25.0f);
	myCamera.yaw = 90.0f;
	myCamera.pitch = 0.0f;
	myCamera.rotate(0.0f, 0.0f);

	transformTorch.SetScale(glm::vec3(0.03f, 0.03f, 0.03f));

	float halfW = (ROOM_WIDTH - DOOR_WIDTH) / 2.1f;

	//=== ROOM 1 WALLS ===
	//Back wall
	wallRoom1Back.SetPos(glm::vec3(0.0f, ROOM_HEIGHT / 2.0f - 1.5f, ROOM1_DEPTH_MIN));
	wallRoom1Back.SetRot(glm::vec3(0.0f, 3.14159f, 0.0f));

	//Left Wall
	wallRoom1Left.SetPos(glm::vec3(-ROOM_WIDTH, ROOM_HEIGHT / 2.0f - 1.5f, (ROOM1_DEPTH_MIN + ROOM1_DEPTH_MAX) / 2.0f));
	wallRoom1Left.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	//Right Wall
	wallRoom1Right.SetPos(glm::vec3(ROOM_WIDTH, ROOM_HEIGHT / 2.0f - 1.5f, (ROOM1_DEPTH_MIN + ROOM1_DEPTH_MAX) / 2.0f));
	wallRoom1Right.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	//Two pieces with a wall-to-door gap
	//Left section (from the left-hand side of the door to the wall)
	wallRoom1FrontL.SetPos(glm::vec3(-DOOR_WIDTH - halfW, ROOM_HEIGHT / 2.0f - 1.5f, ROOM1_DEPTH_MAX));
	wallRoom1FrontL.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));

	//Right-hand side
	wallRoom1FrontR.SetPos(glm::vec3(DOOR_WIDTH + halfW, ROOM_HEIGHT / 2.0f - 1.5f, ROOM1_DEPTH_MAX));
	wallRoom1FrontR.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));

	//Floor
	wallRoom1Floor.SetPos(glm::vec3(0.0f, -1.5f, (ROOM1_DEPTH_MIN + ROOM1_DEPTH_MAX) / 2.0f));
	wallRoom1Floor.SetRot(glm::vec3(-1.5708f, 0.0f, 0.0f));

	//Ceiling
	wallRoom1Ceiling.SetPos(glm::vec3(0.0f, ROOM_HEIGHT - 1.5f, (ROOM1_DEPTH_MIN + ROOM1_DEPTH_MAX) / 2.0f));
	wallRoom1Ceiling.SetRot(glm::vec3(1.5708f, 0.0f, 0.0f));

	//=== BARREL + KEY (ROOM 1 Righ-side)
	transformBarrelKey.SetPos(glm::vec3(20.0f, 0.0f, -15.0f));
	transformBarrelKey.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	transformBarrelKey.SetRot(glm::vec3(0.0f, 0.5f, 0.0f));

	transformKey.SetPos(glm::vec3(20.0f, 1.0f, -15.0f));
	transformKey.SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

	//=== DOOR (between two rooms) ===
	transformDoor.SetPos(glm::vec3(-0.35f, -1.5f, ROOM1_DEPTH_MAX + 0.5f));
	transformDoor.SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
	transformDoor.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));

	//=== ROOM 1 WALL TORCHES ===
	wallTorch1_1.SetPos(glm::vec3(-28.0f, 2.0f, -20.0f));
	wallTorch1_1.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch1_1.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	wallTorch1_2.SetPos(glm::vec3(-28.0f, 2.0f, -5.0f));
	wallTorch1_2.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch1_2.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	wallTorch1_3.SetPos(glm::vec3(28.0f, 2.0f, -20.0f));
	wallTorch1_3.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch1_3.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	wallTorch1_4.SetPos(glm::vec3(28.0f, 2.0f, -5.0f));
	wallTorch1_4.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch1_4.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	//=== ROOM 2 WALLS ===
	wallRoom2Back.SetPos(glm::vec3(0.0f, ROOM_HEIGHT / 2.0f - 1.5f, ROOM2_DEPTH_MAX));
	wallRoom2Back.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));

	wallRoom2Left.SetPos(glm::vec3(-ROOM_WIDTH, ROOM_HEIGHT / 2.0f - 1.5f, (ROOM2_DEPTH_MIN + ROOM2_DEPTH_MAX) / 2.0f));
	wallRoom2Left.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	wallRoom2Right.SetPos(glm::vec3(ROOM_WIDTH, ROOM_HEIGHT / 2.0f - 1.5f, (ROOM2_DEPTH_MIN + ROOM2_DEPTH_MAX) / 2.0f));
	wallRoom2Right.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	wallRoom2Floor.SetPos(glm::vec3(0.0f, -1.5f, (ROOM2_DEPTH_MIN + ROOM2_DEPTH_MAX) / 2.0f));
	wallRoom2Floor.SetRot(glm::vec3(-1.5708f, 0.0f, 0.0f));

	wallRoom2Ceiling.SetPos(glm::vec3(0.0f, ROOM_HEIGHT - 1.5f, (ROOM2_DEPTH_MIN + ROOM2_DEPTH_MAX) / 2.0f));
	wallRoom2Ceiling.SetRot(glm::vec3(1.5708f, 0.0f, 0.0f));

	//=== CHEST + TABLE ===
	transformChest.SetPos(glm::vec3(0.0f, 1.5f, 30.0f));
	transformChest.SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

	transformTable.SetPos(glm::vec3(0.0f, -1.5f, 30.0f));
	transformTable.SetScale(glm::vec3(4.0f, 3.0f, 4.0f));
	transformTable.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	//=== ROOM 2 BARRELS ===
	transformBarrel2_1.SetPos(glm::vec3(-22.0f, 0.0f, 35.0f));
	transformBarrel2_1.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	transformBarrel2_1.SetRot(glm::vec3(0.0f, 0.3f, 0.0f));

	transformBarrel2_2.SetPos(glm::vec3(22.0f, 0.0f, 35.0f));
	transformBarrel2_2.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	transformBarrel2_2.SetRot(glm::vec3(0.0f, 1.5f, 0.0f));

	transformBarrel2_3.SetPos(glm::vec3(-20.0f, 0.0f, 10.0f));
	transformBarrel2_3.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	transformBarrel2_3.SetRot(glm::vec3(0.0f, 2.3f, 0.0f));

	transformBarrel2_4.SetPos(glm::vec3(18.0f, 0.0f, 8.0f));
	transformBarrel2_4.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	transformBarrel2_4.SetRot(glm::vec3(0.1f, 0.8f, 0.0f));

	//=== ROOM 2 WALL TORCHES ===
	wallTorch2_1.SetPos(glm::vec3(-28.0f, 2.0f, 10.0f));
	wallTorch2_1.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch2_1.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	wallTorch2_2.SetPos(glm::vec3(-28.0f, 2.0f, 30.0f));
	wallTorch2_2.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch2_2.SetRot(glm::vec3(0.0f, 1.5708f, 0.0f));

	wallTorch2_3.SetPos(glm::vec3(28.0f, 2.0f, 10.0f));
	wallTorch2_3.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch2_3.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	wallTorch2_4.SetPos(glm::vec3(28.0f, 2.0f, 30.0f));
	wallTorch2_4.SetScale(glm::vec3(0.04f, 0.04f, 0.04f));
	wallTorch2_4.SetRot(glm::vec3(0.0f, -1.5708f, 0.0f));

	//Game Status
	hasKey = false;
	doorOpen = false;
	chestRotation = 0.0f;
	autoRotate = true;
	useToonMode = false;
	firstMouse = true;

	lightPos = glm::vec3(0.0f, 5.0f, -15.0f);
}

void MainGame::linkADS(const Transform& t, const glm::vec3& objectColor, bool useTexture, bool useRim, const glm::vec3& rimColor, float rimPower)
{
	ADS.setMat4("model", t.GetModel());
	glm::mat4 viewMatrix = glm::lookAt(myCamera.getPos(), myCamera.getPos() + myCamera.forward, myCamera.up);
	ADS.setMat4("view", viewMatrix);
	ADS.setVec3("lightPos", lightPos);
	ADS.setVec3("viewPos", myCamera.getPos());
	ADS.setVec3("objectColor", objectColor);
	ADS.setVec3("rimColor", rimColor);
	ADS.setFloat("rimPower", rimPower);
	glUniform1i(glGetUniformLocation(ADS.getProgram(), "useRim"), useRim ? 1 : 0);
	glUniform1i(glGetUniformLocation(ADS.getProgram(), "useTexture"), useTexture ? 1 : 0);
	ADS.setVec3("fogColor", 0.05f, 0.05f, 0.08f);
	ADS.setFloat("fogMinDist", 10.0f);
	ADS.setFloat("fogMaxDist", 40.0f);
	glUniform1i(glGetUniformLocation(ADS.getProgram(), "useFog"), 1);
}

void MainGame::linkToon(const Transform& t, const glm::vec3& objectColor, bool useTexture)
{
	toonShader.setMat4("model", t.GetModel());
	glm::mat4 viewMatrix = glm::lookAt(myCamera.getPos(), myCamera.getPos() + myCamera.forward, myCamera.up);
	toonShader.setMat4("view", viewMatrix);
	toonShader.setVec3("lightPos", lightPos);
	toonShader.setVec3("viewPos", myCamera.getPos());
	toonShader.setVec3("objectColor", objectColor);
	glUniform1i(glGetUniformLocation(toonShader.getProgram(), "useTexture"), useTexture ? 1 : 0);
	toonShader.setVec3("fogColor", 0.05f, 0.05f, 0.08f);
	toonShader.setFloat("fogMinDist", 10.0f);
	toonShader.setFloat("fogMaxDist", 40.0f);
	glUniform1i(glGetUniformLocation(toonShader.getProgram(), "useFog"), 1);
}

void MainGame::processInput()
{
	SDL_Event evnt;
	const float moveSpeed = 5.0f * deltaTime;

	while (SDL_PollEvent(&evnt)) 
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			switch (evnt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				_gameState = GameState::EXIT;
				break;
			case SDLK_r:
				resetScene();
				std::cout << "Scene reseted!" << std::endl;
				break;
			case SDLK_t:
				useToonMode = !useToonMode;
				std::cout << "Shader: " << (useToonMode ? "TOON" : "ADS") << std::endl;
				break;
			case SDLK_SPACE:
				autoRotate = !autoRotate;
				break;
			case SDLK_e:
				//Key collection
				if (!hasKey)
				{
					float distToKey = glm::length(myCamera.pos - transformKey.GetPos());
					if (distToKey < 8.0f)
					{
						hasKey = true;

						//Hide the key
						transformKey.SetPos(glm::vec3(0.0f, -100.0f, 0.0f));
						std::cout << "*** KEY OBTAINED! Go to the door and press E. ***" << std::endl;
					}
					else 
					{
						std::cout << "Get closer to the key! (On the barrel)" << std::endl;
					}
				}
				//Opening the door
				else if (hasKey && !doorOpen)
				{
					float distToDoor = glm::length(myCamera.pos - glm::vec3(0.0f, 0.0f, 0.0f));
					if (distToDoor < 10.0f)
					{
						doorOpen = true;
						//Hide the door
						transformDoor.SetPos(glm::vec3(0.0f, -100.0f, 0.0f));
						std::cout << "*** THE DOOR IS OPEN! Go into the treasure chamber! ***" << std::endl;
					}
					else
					{
						std::cout << "Get closer to the door!" << std::endl;
					}
				}
				break;
			}
			break;
		case SDL_MOUSEMOTION:
		{
			float xOffset = (float)evnt.motion.xrel * mouseSensitivity;
			float yOffset = (float)-evnt.motion.yrel * mouseSensitivity;
			myCamera.rotate(xOffset, yOffset);
			break;
		}
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	glm::vec3 camFwd = glm::normalize(glm::vec3(myCamera.forward.x, 0.0f, myCamera.forward.z));
	glm::vec3 camRt = glm::normalize(glm::cross(camFwd, glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::vec3 newPos = myCamera.pos;

	if (keys[SDL_SCANCODE_W]) newPos += camFwd * moveSpeed;
	if (keys[SDL_SCANCODE_S]) newPos -= camFwd * moveSpeed;
	if (keys[SDL_SCANCODE_A]) newPos -= camRt * moveSpeed;
	if (keys[SDL_SCANCODE_D]) newPos += camRt * moveSpeed;

	//=== WALL BOUNDARY ===
	float margin = 2.0f;

	//X boundary (both room)
	if (newPos.x < -ROOM_WIDTH + margin) newPos.x = -ROOM_WIDTH + margin;
	if (newPos.x > ROOM_WIDTH - margin) newPos.x = ROOM_WIDTH - margin;

	//Z boundary
	if (newPos.z < ROOM1_DEPTH_MIN + margin) newPos.z = ROOM1_DEPTH_MIN + margin;

	//Door control
	if (!doorOpen)
	{
		//Door closed - Stay in Room 1
		if (newPos.z > ROOM1_DEPTH_MAX - margin) newPos.z = ROOM1_DEPTH_MAX - margin;
	}
	else
	{
		//Door opened - You can move to Room 2
		if (newPos.z > ROOM2_DEPTH_MAX - margin) newPos.z = ROOM2_DEPTH_MAX - margin;

		//Check if it fits through the door (narrow passage)
		if (newPos.z > ROOM1_DEPTH_MAX - margin && newPos.z < ROOM2_DEPTH_MIN + margin)
		{
			if (newPos.x < -DOOR_WIDTH || newPos.x > DOOR_WIDTH)
			{
				//Beyond the width of the door - there is a wall
				newPos.z = myCamera.pos.z;
			}
		}
	}

	//Y boundary
	newPos.y = 2.0f;

	//Barrel collision
	glm::vec3 barrelPos = transformBarrelKey.GetPos();
	float distToBarrel = glm::length(glm::vec2(newPos.x - barrelPos.x, newPos.z - barrelPos.z));
	if (distToBarrel < 3.0f)
		newPos = myCamera.pos;

	//Table collision
	glm::vec3 tablePos = transformTable.GetPos();
	float distToTable = glm::length(glm::vec2(newPos.x - tablePos.x, newPos.z - tablePos.z));
	if (distToTable < 4.5f)
		newPos = myCamera.pos;

	myCamera.pos = newPos;

	//Let the camera freeze
	float torchYaw = -glm::radians(myCamera.yaw - 90.0f);
	transformTorch.SetRot(glm::vec3(0.0f, torchYaw, 0.0f));
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		Uint32 currentTime = SDL_GetTicks();
		deltaTime = (currentTime - lastFrameTime) / 1000.0f;
		lastFrameTime = currentTime;
		if (deltaTime > 0.05f) deltaTime = 0.05f;

		processInput();
		drawGame();
	}
}

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.05f, 0.05f, 0.08f, 1.0f);

	//The torch in front of the camera
	glm::vec3 camForward = glm::normalize(glm::vec3(myCamera.forward.x, 0.0f, myCamera.forward.z));
	glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 torchWorldPos = myCamera.pos + camForward * 3.0f + camRight * 1.5f + glm::vec3(0.0f, -1.0f, 0.0f);
	transformTorch.SetPos(torchWorldPos);

	//Flickering flame
	float flicker1 = sin(counter * 15.0f) * 0.3f;
	float flicker2 = sin(counter * 23.0f) * 0.15f;
	float flicker3 = cos(counter * 31.0f) * 0.2f;
	float flickerTotal = flicker1 + flicker2 + flicker3;

	lightPos = glm::vec3(
		torchWorldPos.x + flickerTotal * 0.5f,
		torchWorldPos.y + 4.0f + flickerTotal * 0.4f,
		torchWorldPos.z + flicker2 * 0.4f
	);

	//Chest rotation
	if (autoRotate) chestRotation = counter * 0.3f;
	transformChest.SetRot(glm::vec3(0.0f, chestRotation, 0.0f));

	//Make the key stand out (to catch the eye)
	if (!hasKey)
		transformKey.SetRot(glm::vec3(0.0f, counter * 1.0f, 0.0f));

	float cf = 1.0f + sin(counter * 20.0f) * 0.25f + cos(counter * 27.0f) * 0.15f;

	glDisable(GL_CULL_FACE);

	if (useToonMode)
	{
		toonShader.Bind();
		toonShader.setVec3("lightColor", 1.0f * cf, 0.85f * cf, 0.6f * cf);

		//Player torch
		drawObjectToon(meshTorch, textureTorch, transformTorch, glm::vec3(0.55f, 0.35f, 0.15f), false);

		//=== ROOM 1 ===
		drawObjectToon(meshWall, textureTorch, wallRoom1Back, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshWall, textureTorch, wallRoom1Left, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshWall, textureTorch, wallRoom1Right, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshFloorCeil, textureTorch, wallRoom1Floor, glm::vec3(0.2f, 0.18f, 0.16f), false);
		drawObjectToon(meshFloorCeil, textureTorch, wallRoom1Ceiling, glm::vec3(0.12f, 0.1f, 0.09f), false);

		//Front wall panels (with door openings)
		drawObjectToon(meshWallHalfL, textureTorch, wallRoom1FrontL, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshWallHalfR, textureTorch, wallRoom1FrontR, glm::vec3(0.25f, 0.22f, 0.2f), false);

		//Barrel + Key
		drawObjectToon(meshBarrel, textureBarrel, transformBarrelKey, glm::vec3(0.4f, 0.28f, 0.12f), true);
		if (!hasKey) drawObjectToon(meshKey, textureKey, transformKey, glm::vec3(0.8f, 0.7f, 0.2f), true);

		//Door
		if (!doorOpen) drawObjectToon(meshDoor, textureDoor, transformDoor, glm::vec3(0.4f, 0.25f, 0.1f), true);

		//Wall torches, Room 1
		drawObjectToon(meshTorch, textureTorch, wallTorch1_1, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch1_2, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch1_3, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch1_4, glm::vec3(0.55f, 0.35f, 0.15f), false);

		//=== ROOM 2 ===
		drawObjectToon(meshWall, textureTorch, wallRoom2Back, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshWall, textureTorch, wallRoom2Left, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshWall, textureTorch, wallRoom2Right, glm::vec3(0.25f, 0.22f, 0.2f), false);
		drawObjectToon(meshFloorCeil, textureTorch, wallRoom2Floor, glm::vec3(0.2f, 0.18f, 0.16f), false);
		drawObjectToon(meshFloorCeil, textureTorch, wallRoom2Ceiling, glm::vec3(0.12f, 0.1f, 0.09f), false);

		drawObjectToon(meshChest, textureChest, transformChest, glm::vec3(0.64f, 0.45f, 0.15f), false);
		drawObjectToon(meshTable, textureTable, transformTable, glm::vec3(0.35f, 0.22f, 0.12f), false);

		drawObjectToon(meshBarrel, textureBarrel, transformBarrel2_1, glm::vec3(0.4f, 0.28f, 0.12f), true);
		drawObjectToon(meshBarrel, textureBarrel, transformBarrel2_2, glm::vec3(0.4f, 0.28f, 0.12f), true);
		drawObjectToon(meshBarrel, textureBarrel, transformBarrel2_3, glm::vec3(0.4f, 0.28f, 0.12f), true);
		drawObjectToon(meshBarrel, textureBarrel, transformBarrel2_4, glm::vec3(0.4f, 0.28f, 0.12f), true);

		drawObjectToon(meshTorch, textureTorch, wallTorch2_1, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch2_2, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch2_3, glm::vec3(0.55f, 0.35f, 0.15f), false);
		drawObjectToon(meshTorch, textureTorch, wallTorch2_4, glm::vec3(0.55f, 0.35f, 0.15f), false);
	}
	else
	{
		ADS.Bind();
		ADS.setVec3("lightColor", 1.0f * cf, 0.85f * cf, 0.6f * cf);

		//Player torch
		drawObjectADS(meshTorch, textureTorch, transformTorch, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);

		//=== ROOM 1 WALLS ===
		drawObjectADS(meshWall, textureTorch, wallRoom1Back, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshWall, textureTorch, wallRoom1Left, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshWall, textureTorch, wallRoom1Right, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshFloorCeil, textureTorch, wallRoom1Floor, glm::vec3(0.2f, 0.18f, 0.16f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshFloorCeil, textureTorch, wallRoom1Ceiling, glm::vec3(0.12f, 0.1f, 0.09f), false, false, glm::vec3(0.0f), 2.0f);

		//Front wall panels (with door openings)
		drawObjectADS(meshWallHalfL, textureTorch, wallRoom1FrontL, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshWallHalfR, textureTorch, wallRoom1FrontR, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);

		//Barrel + Key
		drawObjectADS(meshBarrel, textureBarrel, transformBarrelKey, glm::vec3(0.55f, 0.35f, 0.15f), false, true, glm::vec3(0.4f, 0.25f, 0.1f), 2.5f);
		if (!hasKey)
			drawObjectADS(meshKey, textureKey, transformKey, glm::vec3(0.8f, 0.7f, 0.2f), true, true, glm::vec3(1.0f, 0.9f, 0.3f), 2.0f);

		//Door
		if (!doorOpen)
			drawObjectADS(meshDoor, textureDoor, transformDoor, glm::vec3(0.4f, 0.25f, 0.1f), true, false, glm::vec3(0.0f), 2.0f);

		//Wall torches room1
		drawObjectADS(meshTorch, textureTorch, wallTorch1_1, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch1_2, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch1_3, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch1_4, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);

		// === ROOM 2 WALLS ===
		drawObjectADS(meshWall, textureTorch, wallRoom2Back, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshWall, textureTorch, wallRoom2Left, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshWall, textureTorch, wallRoom2Right, glm::vec3(0.25f, 0.22f, 0.2f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshFloorCeil, textureTorch, wallRoom2Floor, glm::vec3(0.2f, 0.18f, 0.16f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshFloorCeil, textureTorch, wallRoom2Ceiling, glm::vec3(0.12f, 0.1f, 0.09f), false, false, glm::vec3(0.0f), 2.0f);

		// Chest (rim lighting) + Table
		drawObjectADS(meshChest, textureChest, transformChest, glm::vec3(0.45f, 0.3f, 0.1f), false, true, glm::vec3(1.5f, 0.9f, 0.3f), 1.5f);
		drawObjectADS(meshTable, textureTable, transformTable, glm::vec3(0.35f, 0.22f, 0.12f), false, true, glm::vec3(1.5f, 0.9f, 0.3f), 1.5f);

		// Room 2 barrels
		drawObjectADS(meshBarrel, textureBarrel, transformBarrel2_1, glm::vec3(0.55f, 0.35f, 0.15f), false, true, glm::vec3(0.4f, 0.25f, 0.1f), 2.5f);
		drawObjectADS(meshBarrel, textureBarrel, transformBarrel2_2, glm::vec3(0.55f, 0.35f, 0.15f), false, true, glm::vec3(0.4f, 0.25f, 0.1f), 2.5f);
		drawObjectADS(meshBarrel, textureBarrel, transformBarrel2_3, glm::vec3(0.55f, 0.35f, 0.15f), false, true, glm::vec3(0.4f, 0.25f, 0.1f), 2.5f);
		drawObjectADS(meshBarrel, textureBarrel, transformBarrel2_4, glm::vec3(0.55f, 0.35f, 0.15f), false, true, glm::vec3(0.4f, 0.25f, 0.1f), 2.5f);

		// Room 2 wall torches
		drawObjectADS(meshTorch, textureTorch, wallTorch2_1, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch2_2, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch2_3, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
		drawObjectADS(meshTorch, textureTorch, wallTorch2_4, glm::vec3(0.55f, 0.35f, 0.15f), false, false, glm::vec3(0.0f), 2.0f);
	}

	glEnable(GL_CULL_FACE);

	//====== FLAME EFFECTS ======
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	flameShader.Bind();
	float billboardYaw = -glm::radians(myCamera.yaw - 90.0f);

	//Player torch flame
	transformFlamePlayer.SetPos(glm::vec3(torchWorldPos.x, torchWorldPos.y + 1.2f, torchWorldPos.z));
	transformFlamePlayer.SetScale(glm::vec3(0.7f + flickerTotal * 0.1f, 0.9f + flickerTotal * 0.1f, 0.7f));
	transformFlamePlayer.SetRot(glm::vec3(0.0f, billboardYaw, 0.0f));
	drawFlame(transformFlamePlayer, 0.0f);

	//Wall torch flames - Room 1
	float wf = sin(counter * 12.0f) * 0.1f;
	Transform ft;
	ft.SetScale(glm::vec3(1.2f + wf, 1.5f + wf, 0.8f));
	ft.SetRot(glm::vec3(0.0f, billboardYaw, 0.0f));

	ft.SetPos(glm::vec3(-28.0f, 3.7f, -20.0f)); drawFlame(ft, 1.0f);
	ft.SetPos(glm::vec3(-28.0f, 3.7f, -5.0f)); drawFlame(ft, 2.0f);
	ft.SetPos(glm::vec3(28.0f, 3.7f, -20.0f)); drawFlame(ft, 3.0f);
	ft.SetPos(glm::vec3(28.0f, 3.7f, -5.0f)); drawFlame(ft, 4.0f);

	//Wall torch flames - Room 2
	ft.SetPos(glm::vec3(-28.0f, 3.7f, 10.0f)); drawFlame(ft, 5.0f);
	ft.SetPos(glm::vec3(-28.0f, 3.7f, 30.0f)); drawFlame(ft, 6.0f);
	ft.SetPos(glm::vec3(28.0f, 3.7f, 10.0f)); drawFlame(ft, 7.0f);
	ft.SetPos(glm::vec3(28.0f, 3.7f, 30.0f)); drawFlame(ft, 8.0f);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	counter += 1.0f * deltaTime;

	_gameDisplay.swapBuffer();
}