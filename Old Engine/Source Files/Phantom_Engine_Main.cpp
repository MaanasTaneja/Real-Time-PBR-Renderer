#include "PhantomEngine.h"

#include <iostream>
#include <thread>


extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;            //Helps run shader code exclusively on the nvidia card 
}



// camera
int counter = 0;
Camera camera(glm::vec3(0.0f, -1.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, MasterRenderer* ALBIAN);
void processPlayerInput(GLFWwindow* window, MasterRenderer* Karlson);
void processUIinput(GLFWwindow* window, MasterRenderer* Karlson, LevelBuilder* Level1);

float playerForce = 130.0f * 60;
float maxSpeed = 1000.0f;
bool isJumping = false;
Model* tableModel;
Model* monitorModel;
Model* cabinetModel;

Model* currentModel;
Grapple player;

vector<std::string> faces
{
	"resources/textures/skybox/posX.png",
	"resources/textures/skybox/negX.png",
	"resources/textures/skybox/posY.png",
	"resources/textures/skybox/negY.png",
	"resources/textures/skybox/posZ.png",
	"resources/textures/skybox/negZ.png"
};


/*
vector<std::string> faces
{
	"resources/textures/skybox/right.jpg",
	"resources/textures/skybox/left.jpg",
	"resources/textures/skybox/top.jpg",
	"resources/textures/skybox/bottom.jpg",
	"resources/textures/skybox/front.jpg",
	"resources/textures/skybox/back.jpg",
};
*/

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BOI", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	bool placed = false;
	bool placed2 = false;
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	LevelBuilder Level1;
	Level1.LevelCamera = &camera;

	

	MasterRenderer* Karlson;   
	Karlson = Level1.buildLevelFromFile("resources/level3.txt");

	LiquidBody* water = new LiquidBody();
	water->generateTileLiquidMesh(0, -2, 10.0f, 56);
	//water->makeLiquidFromMesh(monitorModel);
	water->addLiquidTexture("resources/textures/lava/LavaTex.png");
	water->flowLiquid(glm::vec3(-1.0f));
	water->setAlpha(1.0f);
	water->setTileFactor(10.0f);
	water->setAdditive(true);
	water->setWaveSpeed(0.5f);


	Karlson->addLiquid(water);

	LiquidBody* water2 = new LiquidBody();
	water2->generateTileLiquidMesh(0, -1, 10.0f, 56);
	//water->makeLiquidFromMesh(monitorModel);
	water2->addLiquidTexture("resources/textures/lava/LavaTex.png");
	water2->flowLiquid(glm::vec3(-1.0f));
	water2->setAlpha(1.0f);
	water2->setTileFactor(10.0f);
	water2->setAdditive(true);
	water2->setWaveSpeed(0.5f);


	Karlson->addLiquid(water2);

	LiquidBody* water3 = new LiquidBody();
	water3->generateTileLiquidMesh(-1, -2, 10.0f, 56);
	//water->makeLiquidFromMesh(monitorModel);
	water3->addLiquidTexture("resources/textures/lava/LavaTex.png");
	water3->flowLiquid(glm::vec3(-1.0f));
	water3->setAlpha(1.0f);
	water3->setTileFactor(10.0f);
	water3->setAdditive(true);
	water3->setWaveSpeed(0.5f);


	Karlson->addLiquid(water3);

	LiquidBody* water4 = new LiquidBody();
	water4->generateTileLiquidMesh(-1, -1, 10.0f, 56);
	//water->makeLiquidFromMesh(monitorModel);
	water4->addLiquidTexture("resources/textures/lava/LavaTex.png");
	water4->flowLiquid(glm::vec3(-1.0f));
	water4->setAlpha(1.0f);
	water4->setTileFactor(10.0f);
	water4->setAdditive(true);
	water4->setWaveSpeed(0.5f);


	Karlson->addLiquid(water4);
	

	player.player = Karlson->player.player;
	player.lineRender = &Karlson->LineRender;
	player.loadedWorld = Karlson->PhysicsWorld.world;

	Karlson->guncontainer.playerCamera = &camera;
	Karlson->guncontainer.loadedWorld = Karlson->PhysicsWorld.world;
	Gun* first = new Gun(false, 0.2f, 40.0f, 2.0f, 1, glm::vec3(0.0f, 0.0f, 0.0f));
	first->position = glm::vec3(0.164968, 2.4163, -1.73515);
	first->gunModel = new Model("resources/gameObjects/guns/pistol5.gltf");
	first->loadedWorld = Karlson->PhysicsWorld.world;
	first->genGunRigid();

	Karlson->addGun(first);


	Model* greenbox = new Model("resources/gameObjects/monitor/file_cabinets/lol/table/table6.obj");
	bool hask = false;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Karlson->addDebugEntity(currentModel , counter);
		processInput(window, Karlson);
		
		processPlayerInput(window, Karlson);
		
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		{
			if (hask == true)
			{
				hask = false;
			}
			else {
				Entity* green = new Entity(greenbox, glm::vec3(camera.getPosition().x, camera.getPosition().y +0.1f, camera.getPosition().z), glm::vec3(0.1f), 90.0f, 1.0f);
				Karlson->addEntities(green);
			
				hask = true;
			}
		}



		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		Karlson->render(camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

bool t = false;
bool hasPressed = false;

void processInput(GLFWwindow* window, MasterRenderer* ALBIAN)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		ALBIAN->guncontainer.pickupGun();
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		ALBIAN->guncontainer.throwGun();
	}

	ALBIAN->player.player->rigidBodyMovement(window, ALBIAN->PhysicsWorld.world , &camera , deltaTime);
	//ALBIAN->player.player->kinematicMovement(window, &camera, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//camera.ProcessMouseScroll(yoffset);
	counter = counter + yoffset;
	if (counter == 1)
	{
		currentModel = cabinetModel;
		cout << "CABINET" << endl;
	}
	else if (counter == 2)
	{
		currentModel = tableModel;
		cout << "tabkl" << endl;
	}

	else {
		counter = 0;
		currentModel = monitorModel;
		cout << "moni" << endl;
	}
}
bool mouse = false;

void processUIinput(GLFWwindow* window, MasterRenderer* Karlson, LevelBuilder* Level1)
{


	btVector3 start = btVector3(camera.Position.x, camera.Position.y, camera.Position.z);
	btVector3 end = btVector3(camera.Front.x, camera.Front.y, camera.Front.z) * 1000.0f;

	btCollisionWorld::ClosestRayResultCallback raycall(start, end);

	Karlson->PhysicsWorld.world->rayTest(start, end, raycall);
	glm::vec3 hitPoint = glm::vec3(raycall.m_hitPointWorld.getX(), raycall.m_hitPointWorld.getY()+0.09f, raycall.m_hitPointWorld.getZ());
	Karlson->rayPoint = hitPoint;


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{


		if (mouse == true)
		{

		}
		else {
			mouse = true;

			if (raycall.hasHit() == true)
			{
				Entity* ent;
				if (counter == 1)
				{
					ent = new Entity(currentModel, hitPoint, glm::vec3(0.004f), 0.0f, 2.0f);
				}

				else if (counter == 2)
				{
					ent = new Entity(currentModel, hitPoint, glm::vec3(0.15f), 0.0f, 2.0f);
				}
				else {
					ent = new Entity(currentModel, hitPoint, glm::vec3(0.2f), 0.0f, 2.0f);
				}

				ent->extraMaterials = false;

				Level1->writeToLevelFile(ent);
				Karlson->addEntities(ent);
			}
		}

	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
	{
		mouse = false;
	}
}

bool isLocked = false;
bool isIdle = false;
void processPlayerInput(GLFWwindow* window, MasterRenderer* Karlson)
{

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
			if (isLocked == false)
			{
				isLocked = true;
				player.playCamera = &camera;
				player.projectGrapple(camera.Position, camera.Front, glm::vec3(1.0f, 1.0f, 1.0f), 6.0f);
				player.update();

				
			}
			else {

				player.update();

			}
		
	}


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		if (Karlson->guncontainer.isGunEquiped == true)
		{
			if (isIdle == false)
			{
				isIdle = true;
				Karlson->guncontainer.activeGun->shoot();
			}
			else
			{

			}

		}
		else {
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
	{
		isIdle = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
	{
		player.cleanRenderer();
		isLocked = false;
	}
}








