/*---------------------------------------------------------*/
/* ------------  Proyecto Final: Pueblo Vaquero -----------*/
/*------------------------	2021-2	-----------------------*/
/*------------- Alumnos:    Sebas Arjona          ---------*/
/*--------------- Ramos Villaseñor César Mauricio ---------*/
/*--------------- Sandoval Miramontes Joaquín -------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

// Variables for tumbleweed animation
bool	playTumbleWeed = false,
		tumbleUp = false;
float	tumblePosX = -9.0f,
		tumblePosY = -0.1f,
		tumblePosZ = 50.0f,
		tumbleRotY = -180.0f,
		tumbleRotX = 0.0f;
int		tumbleState = 0,
		tumbleShootCount = 0;

//Variables for the cow animation
float	cowPosX = 0.0f,
		cowPosY = 0.0f,
		cowPosZ = 0.0f,
		cowRotY = 0.0f,
		cowRotBLX = 0.0f,
		cowRotFLX = 0.0f,
		cowRotBLZ = 0.0f,
		cowRotFLZ = 0.0f,
		cowPosBLX = -0.2f,
		cowPosBLZ = 0.506f,
		cowPosBRX = 0.1f,
		cowPosBRZ = 0.59f,
		cowPosFLX = -0.2f,
		cowPosFLZ = -0.425f,
		cowPosFRX = 0.15f,
		cowPosFRZ = -0.430;

float	cowIncX = 0.0f,
		cowIncY = 0.0f,
		cowIncZ = 0.0f,
		cowRotIncY = 0.0f,
		cowRotIncBLX = 0.0f,
		cowRotIncFLX = 0.0f,
		cowRotIncBLZ = 0.0f,
		cowRotIncFLZ = 0.0f,
		cowIncBLX = -0.2f,
		cowIncBLZ = 0.506f,
		cowIncBRX = 0.1f,
		cowIncBRZ = 0.59f,
		cowIncFLX = -0.2f,
		cowIncFLZ = -0.425f,
		cowIncFRX = 0.15f,
		cowIncFRZ = -0.430;


#define COW_MAX_FRAMES 24
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _cow_frame
{
	//Variables para GUARDAR Key Frames
	float cowPosX;		//Variable para PosicionX
	float cowPosY;		//Variable para PosicionY
	float cowPosZ;		//Variable para PosicionZ
	float cowRotY;
	float cowRotBLX;
	float cowRotFLX;
	float cowRotBLZ;
	float cowRotFLZ;
	float cowPosBLX;
	float cowPosBLZ;
	float cowPosBRX;
	float cowPosBRZ;
	float cowPosFLX;
	float cowPosFLZ;
	float cowPosFRX;
	float cowPosFRZ;



}COW_FRAME;

COW_FRAME CowKeyFrame[COW_MAX_FRAMES];
int CowFrameIndex = 24;			//introducir datos
bool cowPlay = false;
int cowPlayIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << CowFrameIndex << std::endl;

	CowKeyFrame[CowFrameIndex].cowPosX = cowPosX;
	CowKeyFrame[CowFrameIndex].cowPosY = cowPosY;
	CowKeyFrame[CowFrameIndex].cowPosZ = cowPosZ;
	CowKeyFrame[CowFrameIndex].cowRotY = cowRotY;
	CowKeyFrame[CowFrameIndex].cowRotBLX = cowRotBLX;
	CowKeyFrame[CowFrameIndex].cowRotFLX = cowRotFLX;
	CowKeyFrame[CowFrameIndex].cowRotBLZ = cowRotBLZ;
	CowKeyFrame[CowFrameIndex].cowRotFLZ = cowRotFLZ;
	CowKeyFrame[CowFrameIndex].cowPosBLX = cowPosBLX;
	CowKeyFrame[CowFrameIndex].cowPosBLZ = cowPosBLZ;
	CowKeyFrame[CowFrameIndex].cowPosBRX = cowPosBRX;
	CowKeyFrame[CowFrameIndex].cowPosBRZ = cowPosBRZ;
	CowKeyFrame[CowFrameIndex].cowPosFRX = cowPosFRX;
	CowKeyFrame[CowFrameIndex].cowPosFRZ = cowPosFRZ;
	CowKeyFrame[CowFrameIndex].cowPosFLX = cowPosFLX;
	CowKeyFrame[CowFrameIndex].cowPosFLZ = cowPosFLZ;

	CowFrameIndex++;
}

void resetElements(void)
{
	cowPosX = CowKeyFrame[0].cowPosX;
	cowPosY = CowKeyFrame[0].cowPosY;
	cowPosZ = CowKeyFrame[0].cowPosZ;
	cowRotY = CowKeyFrame[0].cowRotY;
	cowRotBLX = CowKeyFrame[0].cowRotBLX;
	cowRotFLX = CowKeyFrame[0].cowRotFLX;
	cowRotBLZ = CowKeyFrame[0].cowRotBLZ;
	cowRotFLZ = CowKeyFrame[0].cowRotFLZ;
	cowPosBLX = CowKeyFrame[0].cowPosBLX;
	cowPosBLZ = CowKeyFrame[0].cowPosBLZ;
	cowPosBRX = CowKeyFrame[0].cowPosBRX;
	cowPosBRZ = CowKeyFrame[0].cowPosBRZ;
	cowPosFLX = CowKeyFrame[0].cowPosFLX;
	cowPosFLZ = CowKeyFrame[0].cowPosFLZ;
	cowPosFRX = CowKeyFrame[0].cowPosFRX;
	cowPosFRZ = CowKeyFrame[0].cowPosFRZ;

}

void interpolation(void)
{
	cowIncX = (CowKeyFrame[cowPlayIndex + 1].cowPosX - CowKeyFrame[cowPlayIndex].cowPosX) / i_max_steps;
	cowIncY = (CowKeyFrame[cowPlayIndex + 1].cowPosY - CowKeyFrame[cowPlayIndex].cowPosY) / i_max_steps;
	cowIncZ = (CowKeyFrame[cowPlayIndex + 1].cowPosZ - CowKeyFrame[cowPlayIndex].cowPosZ) / i_max_steps;
	cowRotIncY = (CowKeyFrame[cowPlayIndex + 1].cowRotY - CowKeyFrame[cowPlayIndex].cowRotY) / i_max_steps;
	cowRotIncBLX = (CowKeyFrame[cowPlayIndex + 1].cowRotBLX - CowKeyFrame[cowPlayIndex].cowRotBLX) / i_max_steps;
	cowRotIncFLX = (CowKeyFrame[cowPlayIndex + 1].cowRotFLX - CowKeyFrame[cowPlayIndex].cowRotFLX) / i_max_steps;
	cowRotIncBLZ = (CowKeyFrame[cowPlayIndex + 1].cowRotBLZ - CowKeyFrame[cowPlayIndex].cowRotBLZ) / i_max_steps;
	cowRotIncFLZ = (CowKeyFrame[cowPlayIndex + 1].cowRotFLZ - CowKeyFrame[cowPlayIndex].cowRotFLZ) / i_max_steps;
	cowIncBLX = (CowKeyFrame[cowPlayIndex + 1].cowPosBLX - CowKeyFrame[cowPlayIndex].cowPosBLX) / i_max_steps;
	cowIncBLZ = (CowKeyFrame[cowPlayIndex + 1].cowPosBLZ - CowKeyFrame[cowPlayIndex].cowPosBLZ) / i_max_steps;
	cowIncBRX = (CowKeyFrame[cowPlayIndex + 1].cowPosBRX - CowKeyFrame[cowPlayIndex].cowPosBRX) / i_max_steps;
	cowIncBRZ = (CowKeyFrame[cowPlayIndex + 1].cowPosBRZ - CowKeyFrame[cowPlayIndex].cowPosBRZ) / i_max_steps;
	cowIncFLX = (CowKeyFrame[cowPlayIndex + 1].cowPosFLX - CowKeyFrame[cowPlayIndex].cowPosFLX) / i_max_steps;
	cowIncFLZ = (CowKeyFrame[cowPlayIndex + 1].cowPosFLZ - CowKeyFrame[cowPlayIndex].cowPosFLZ) / i_max_steps;
	cowIncFRX = (CowKeyFrame[cowPlayIndex + 1].cowPosFRX - CowKeyFrame[cowPlayIndex].cowPosFRX) / i_max_steps;
	cowIncFRZ = (CowKeyFrame[cowPlayIndex + 1].cowPosFRZ - CowKeyFrame[cowPlayIndex].cowPosFRZ) / i_max_steps;
}

void animate(void)
{
	if (playTumbleWeed) {
		
		if (tumbleState == 1) {
			tumblePosZ -= 0.05f;
			if (tumbleUp) {
				tumblePosY += 0.05f;
				
				if (tumblePosY >= 0.6f) {
					tumbleUp = false;
				}
			}
			else if (!tumbleUp) {
				tumblePosY -= 0.05f;
				if (tumblePosY <= -0.1f) {
					tumbleUp = true;
				}
			}
			if (tumblePosZ <= 34.0f && tumblePosY <= 0.1f) {
				tumbleState = 2;
				bool played = PlaySound(L"western.wav", NULL, SND_ASYNC);
				cout << "Ambient:" << played << endl;
			}

		}
		else if (tumbleState == 2) {
			tumbleRotY += 4.0f;
			if (tumbleRotY >= -90.0f) {
				tumbleState = 3;
			}
		}
		else if (tumbleState == 3) {
			tumblePosX -= 0.05f;
			if (tumbleUp) {
				tumblePosY += 0.05f;

				if (tumblePosY >= 0.6f) {
					tumbleUp = false;
				}
			}
			else if (!tumbleUp) {
				tumblePosY -= 0.05f;
				if (tumblePosY <= -0.1f) {
					tumbleUp = true;
				}
			}
			if (tumblePosX <= -29.0f && tumblePosY <= 0.1f) {
				tumbleState = 4;
			}
		}

		else if (tumbleState == 4) {
			tumbleRotY -= 4.0f;
			if (tumbleRotY <= -180.0f) {
				tumbleState = 5;
				bool played = PlaySound(L"shoots.wav", NULL, SND_ASYNC);
				cout << "Ambient:" << played << endl;
			}
		}
		else if (tumbleState == 5) {
			if (tumbleUp) {
				tumbleRotX -= 4.0f;
				if (tumbleRotX <= -50.0f) {
					tumbleUp = false;
				}
			}
			else if (!tumbleUp) {
				tumbleRotX += 4.0f;
				if (tumbleRotX >= 0.0f) {
					tumbleUp = true;
					tumbleShootCount++;
				}
			}
			if (tumbleShootCount == 4) {
				tumbleState = 0;
			}
		}
	}
	
	if (cowPlay)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			cowPlayIndex++;
			if (cowPlayIndex > CowFrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				cowPlayIndex = 0;
				cowPlay = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			cowPosX += cowIncX;
			cowPosY += cowIncY;
			cowPosZ += cowIncZ;
			cowRotY += cowRotIncY;
			cowRotBLX += cowRotIncBLX;
			cowRotFLX += cowRotIncFLX;
			cowRotBLZ += cowRotIncBLZ;
			cowRotFLZ += cowRotIncFLZ;
			cowPosBLX += cowIncBLX;
			cowPosBLZ += cowIncBLZ;
			cowPosBRX += cowIncBRX;
			cowPosBRZ += cowIncBRZ;
			cowPosFRX += cowIncFRX;
			cowPosFRZ += cowIncFRZ;
			cowPosFLX += cowIncFLX;
			cowPosFLZ += cowIncFLZ;


			i_curr_steps++;
		}
	}

	//Vehículo
	//if (animacion)
	//{
	//	
	//}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");


	/*
		left	-x
		front	+z
		right	+x
		back	-z
		top		+y
		bottom	-y
	*/
	vector<std::string> faces
	{
		"resources/skybox/Desierto7/posx.jpg",		//right
		"resources/skybox/Desierto7/negx.jpg",		//left
		"resources/skybox/Desierto7/posy.jpg",		//top
		"resources/skybox/Desierto7/negy.jpg",		//bottom
		"resources/skybox/Desierto7/posz.jpg",		//front
		"resources/skybox/Desierto7/negz.jpg"		//back
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model woodFence("resources/objects/woodFence/woodFence.obj");
	Model rock1("resources/objects/rock1/rock1.obj");
	Model barrel1("resources/objects/barrel/barrel.obj");
	Model barn("resources/objects/barn/barn.obj");
	Model tree1("resources/objects/tree1/Gledista_Triacanthos.obj");
	Model tree2("resources/objects/tree2/Gledista_Triacanthos_2.obj");
	//Model tree3("resources/objects/tree3/Gledista_Triacanthos_3.obj");
	Model windmill("resources/objects/windmill/windmill.obj");
	Model waterTower("resources/objects/waterTower/waterTower.obj");
	Model cactus("resources/objects/cactus/cactus.obj");
	Model tipi("resources/objects/tipi/tipi.obj");
	Model cow("resources/objects/cow/cow.obj");
	Model bull("resources/objects/bull/bull.obj");
	Model ground("resources/objects/piso/superficie.obj");
	Model saloon("resources/objects/saloon/saloon.obj");
	Model sheriff_office("resources/objects/sheriff/sheriff.obj");
	Model hotel("resources/objects/hotel/hotel.obj");
	Model train_station("resources/objects/trainStation/trainStation.obj");
	Model bank("resources/objects/bank/bank.obj");
	Model store("resources/objects/store/store.obj");
	Model church("resources/objects/church/church.obj");
	Model gallow("resources/objects/gallow/gallow.obj");
	Model house("resources/objects/house1/house1.obj");
	Model house2("resources/objects/house2/house2.obj");
	Model house3("resources/objects/house3/house3.obj");
	Model house4("resources/objects/house4/house4.obj");
	Model house5("resources/objects/house5/house5.obj");
	Model rail("resources/objects/rail/rail.obj");
	Model doctor("resources/objects/doctor/doctor.obj");
	Model tunnel("resources/objects/tunnel/tunnel.obj");
	//Model monteUno("resources/objects/monte1/monte1.obj");
	//Model monteDos("resources/objects/monte2/monte2.obj");
	Model mountain("resources/objects/mountain/mountain.obj");

	//Models for animations
	Model cowBody("resources/objects/cow/cow_body.obj");
	Model cowBackRightLeg("resources/objects/cow/cow_back_right_leg.obj");
	Model cowBackLeftLeg("resources/objects/cow/cow_back_left_leg.obj");
	Model cowFrontRightLeg("resources/objects/cow/cow_front_right_leg.obj");
	Model cowFrontLeftLeg("resources/objects/cow/cow_front_left_leg.obj");
	Model tumbleWeed("resources/objects/tumbleWeed/tumbleweed.obj");


	CowKeyFrame[0].cowPosX = 0;
	CowKeyFrame[0].cowPosY = 0;
	CowKeyFrame[0].cowPosZ = 0;
	CowKeyFrame[0].cowRotY = 0;
	CowKeyFrame[0].cowRotBLX = 0;
	CowKeyFrame[0].cowRotFLX = 0;
	CowKeyFrame[0].cowRotBLZ = 0;
	CowKeyFrame[0].cowRotFLZ = 0;
	CowKeyFrame[0].cowPosBLX = -0.2f;
	CowKeyFrame[0].cowPosBLZ = 0.506f;
	CowKeyFrame[0].cowPosBRX = 0.1f;
	CowKeyFrame[0].cowPosBRZ = 0.59f;
	CowKeyFrame[0].cowPosFRX = 0.15f;
	CowKeyFrame[0].cowPosFRZ = -0.430f;
	CowKeyFrame[0].cowPosFLX = -0.2f;
	CowKeyFrame[0].cowPosFLZ = -0.425f;

	CowKeyFrame[1].cowPosX = 0;
	CowKeyFrame[1].cowPosY = 0;
	CowKeyFrame[1].cowPosZ = -1;
	CowKeyFrame[1].cowRotY = 0;
	CowKeyFrame[1].cowRotBLX = 15;
	CowKeyFrame[1].cowRotFLX = -15;
	CowKeyFrame[1].cowRotBLZ = 0;
	CowKeyFrame[1].cowRotFLZ = 0;
	CowKeyFrame[1].cowPosBLX = -0.2f;
	CowKeyFrame[1].cowPosBLZ = 0.506f;
	CowKeyFrame[1].cowPosBRX = 0.1f;
	CowKeyFrame[1].cowPosBRZ = 0.59f;
	CowKeyFrame[1].cowPosFRX = 0.15f;
	CowKeyFrame[1].cowPosFRZ = -0.430f;
	CowKeyFrame[1].cowPosFLX = -0.2f;
	CowKeyFrame[1].cowPosFLZ = -0.425f;
	
	CowKeyFrame[2].cowPosX = 0;
	CowKeyFrame[2].cowPosY = 0;
	CowKeyFrame[2].cowPosZ = -2;
	CowKeyFrame[2].cowRotY = 0;
	CowKeyFrame[2].cowRotBLX = 35;
	CowKeyFrame[2].cowRotFLX = -35;
	CowKeyFrame[2].cowRotBLZ = 0;
	CowKeyFrame[2].cowRotFLZ = 0;
	CowKeyFrame[2].cowPosBLX = -0.2f;
	CowKeyFrame[2].cowPosBLZ = 0.506f;
	CowKeyFrame[2].cowPosBRX = 0.1f;
	CowKeyFrame[2].cowPosBRZ = 0.59f;
	CowKeyFrame[2].cowPosFRX = 0.15f;
	CowKeyFrame[2].cowPosFRZ = -0.430f;
	CowKeyFrame[2].cowPosFLX = -0.2f;
	CowKeyFrame[2].cowPosFLZ = -0.425f;
	
	CowKeyFrame[3].cowPosX = 0;
	CowKeyFrame[3].cowPosY = 0;
	CowKeyFrame[3].cowPosZ = -3;
	CowKeyFrame[3].cowRotY = 0;
	CowKeyFrame[3].cowRotBLX = 0;
	CowKeyFrame[3].cowRotFLX = 0;
	CowKeyFrame[3].cowRotBLZ = 0;
	CowKeyFrame[3].cowRotFLZ = 0;
	CowKeyFrame[3].cowPosBLX = -0.2f;
	CowKeyFrame[3].cowPosBLZ = 0.506f;
	CowKeyFrame[3].cowPosBRX = 0.1f;
	CowKeyFrame[3].cowPosBRZ = 0.59f;
	CowKeyFrame[3].cowPosFRX = 0.15f;
	CowKeyFrame[3].cowPosFRZ = -0.430f;
	CowKeyFrame[3].cowPosFLX = -0.2f;
	CowKeyFrame[3].cowPosFLZ = -0.425f;
	
	CowKeyFrame[4].cowPosX = 0;
	CowKeyFrame[4].cowPosY = 0;
	CowKeyFrame[4].cowPosZ = -4;
	CowKeyFrame[4].cowRotY = 0;
	CowKeyFrame[4].cowRotBLX = -15;
	CowKeyFrame[4].cowRotFLX = 15;
	CowKeyFrame[4].cowRotBLZ = 0;
	CowKeyFrame[4].cowRotFLZ = 0;
	CowKeyFrame[4].cowPosBLX = -0.2f;
	CowKeyFrame[4].cowPosBLZ = 0.506f;
	CowKeyFrame[4].cowPosBRX = 0.1f;
	CowKeyFrame[4].cowPosBRZ = 0.59f;
	CowKeyFrame[4].cowPosFRX = 0.15f;
	CowKeyFrame[4].cowPosFRZ = -0.430f;
	CowKeyFrame[4].cowPosFLX = -0.2f;
	CowKeyFrame[4].cowPosFLZ = -0.425f;
	
	CowKeyFrame[5].cowPosX = 0;
	CowKeyFrame[5].cowPosY = 0;
	CowKeyFrame[5].cowPosZ = -5;
	CowKeyFrame[5].cowRotY = 0;
	CowKeyFrame[5].cowRotBLX = -35;
	CowKeyFrame[5].cowRotFLX = 35;
	CowKeyFrame[5].cowRotBLZ = 0;
	CowKeyFrame[5].cowRotFLZ = 0;
	CowKeyFrame[5].cowPosBLX = -0.2f;
	CowKeyFrame[5].cowPosBLZ = 0.506f;
	CowKeyFrame[5].cowPosBRX = 0.1f;
	CowKeyFrame[5].cowPosBRZ = 0.59f;
	CowKeyFrame[5].cowPosFRX = 0.15f;
	CowKeyFrame[5].cowPosFRZ = -0.430f;
	CowKeyFrame[5].cowPosFLX = -0.2f;
	CowKeyFrame[5].cowPosFLZ = -0.425f;
	
	CowKeyFrame[6].cowPosX = 0;
	CowKeyFrame[6].cowPosY = 0;
	CowKeyFrame[6].cowPosZ = -6;
	CowKeyFrame[6].cowRotY = 0;
	CowKeyFrame[6].cowRotBLX = 0;
	CowKeyFrame[6].cowRotFLX = 0;
	CowKeyFrame[6].cowRotBLZ = 0;
	CowKeyFrame[6].cowRotFLZ = 0;
	CowKeyFrame[6].cowPosBLX = -0.2f;
	CowKeyFrame[6].cowPosBLZ = 0.506f;
	CowKeyFrame[6].cowPosBRX = 0.1f;
	CowKeyFrame[6].cowPosBRZ = 0.59f;
	CowKeyFrame[6].cowPosFRX = 0.15f;
	CowKeyFrame[6].cowPosFRZ = -0.430f;
	CowKeyFrame[6].cowPosFLX = -0.2f;
	CowKeyFrame[6].cowPosFLZ = -0.425f;
	
	CowKeyFrame[7].cowPosX = 0;
	CowKeyFrame[7].cowPosY = 0;
	CowKeyFrame[7].cowPosZ = -6;
	CowKeyFrame[7].cowRotY = -90;
	CowKeyFrame[7].cowRotBLX = 0;
	CowKeyFrame[7].cowRotFLX = 0;
	CowKeyFrame[7].cowRotBLZ = 0;
	CowKeyFrame[7].cowRotFLZ = 0;
	CowKeyFrame[7].cowPosBLX = -0.506f;
	CowKeyFrame[7].cowPosBLZ = -0.2f;
	CowKeyFrame[7].cowPosBRX = -0.59f;
	CowKeyFrame[7].cowPosBRZ = 0.1f;
	CowKeyFrame[7].cowPosFRX = 0.430f;
	CowKeyFrame[7].cowPosFRZ = 0.15f;
	CowKeyFrame[7].cowPosFLX = 0.425f;
	CowKeyFrame[7].cowPosFLZ = -0.2f;
	
	CowKeyFrame[8].cowPosX = 1;
	CowKeyFrame[8].cowPosY = 0;
	CowKeyFrame[8].cowPosZ = -6;
	CowKeyFrame[8].cowRotY = -90;
	CowKeyFrame[8].cowRotBLX = 0;
	CowKeyFrame[8].cowRotFLX = 0;
	CowKeyFrame[8].cowRotBLZ = 15;
	CowKeyFrame[8].cowRotFLZ = -15;
	CowKeyFrame[8].cowPosBLX = -0.506f;
	CowKeyFrame[8].cowPosBLZ = -0.2f;
	CowKeyFrame[8].cowPosBRX = -0.59f;
	CowKeyFrame[8].cowPosBRZ = 0.1f;
	CowKeyFrame[8].cowPosFRX = 0.430f;
	CowKeyFrame[8].cowPosFRZ = 0.15f;
	CowKeyFrame[8].cowPosFLX = 0.425f;
	CowKeyFrame[8].cowPosFLZ = -0.2f;
	
	CowKeyFrame[9].cowPosX = 2;
	CowKeyFrame[9].cowPosY = 0;
	CowKeyFrame[9].cowPosZ = -6;
	CowKeyFrame[9].cowRotY = -90;
	CowKeyFrame[9].cowRotBLX = 0;
	CowKeyFrame[9].cowRotFLX = 0;
	CowKeyFrame[9].cowRotBLZ = 35;
	CowKeyFrame[9].cowRotFLZ = -35;
	CowKeyFrame[9].cowPosBLX = -0.506f;
	CowKeyFrame[9].cowPosBLZ = -0.2f;
	CowKeyFrame[9].cowPosBRX = -0.59f;
	CowKeyFrame[9].cowPosBRZ = 0.1f;
	CowKeyFrame[9].cowPosFRX = 0.430f;
	CowKeyFrame[9].cowPosFRZ = 0.15f;
	CowKeyFrame[9].cowPosFLX = 0.425f;
	CowKeyFrame[9].cowPosFLZ = -0.2f;
	
	CowKeyFrame[10].cowPosX = 3;
	CowKeyFrame[10].cowPosY = 0;
	CowKeyFrame[10].cowPosZ = -6;
	CowKeyFrame[10].cowRotY = -90;
	CowKeyFrame[10].cowRotBLX = 0;
	CowKeyFrame[10].cowRotFLX = 0;
	CowKeyFrame[10].cowRotBLZ = 0;
	CowKeyFrame[10].cowRotFLZ = 0;
	CowKeyFrame[10].cowPosBLX = -0.506f;
	CowKeyFrame[10].cowPosBLZ = -0.2f;
	CowKeyFrame[10].cowPosBRX = -0.59f;
	CowKeyFrame[10].cowPosBRZ = 0.1f;
	CowKeyFrame[10].cowPosFRX = 0.430f;
	CowKeyFrame[10].cowPosFRZ = 0.15f;
	CowKeyFrame[10].cowPosFLX = 0.425f;
	CowKeyFrame[10].cowPosFLZ = -0.2f;
	
	CowKeyFrame[11].cowPosX = 4;
	CowKeyFrame[11].cowPosY = 0;
	CowKeyFrame[11].cowPosZ = -6;
	CowKeyFrame[11].cowRotY = -90;
	CowKeyFrame[11].cowRotBLX = 0;
	CowKeyFrame[11].cowRotFLX = 0;
	CowKeyFrame[11].cowRotBLZ = -15;
	CowKeyFrame[11].cowRotFLZ = 15;
	CowKeyFrame[11].cowPosBLX = -0.506f;
	CowKeyFrame[11].cowPosBLZ = -0.2f;
	CowKeyFrame[11].cowPosBRX = -0.59f;
	CowKeyFrame[11].cowPosBRZ = 0.1f;
	CowKeyFrame[11].cowPosFRX = 0.430f;
	CowKeyFrame[11].cowPosFRZ = 0.15f;
	CowKeyFrame[11].cowPosFLX = 0.425f;
	CowKeyFrame[11].cowPosFLZ = -0.2f;
	
	CowKeyFrame[12].cowPosX = 5;
	CowKeyFrame[12].cowPosY = 0;
	CowKeyFrame[12].cowPosZ = -6;
	CowKeyFrame[12].cowRotY = -90;
	CowKeyFrame[12].cowRotBLX = 0;
	CowKeyFrame[12].cowRotFLX = 0;
	CowKeyFrame[12].cowRotBLZ = -35;
	CowKeyFrame[12].cowRotFLZ = 35;
	CowKeyFrame[12].cowPosBLX = -0.506f;
	CowKeyFrame[12].cowPosBLZ = -0.2f;
	CowKeyFrame[12].cowPosBRX = -0.59f;
	CowKeyFrame[12].cowPosBRZ = 0.1f;
	CowKeyFrame[12].cowPosFRX = 0.430f;
	CowKeyFrame[12].cowPosFRZ = 0.15f;
	CowKeyFrame[12].cowPosFLX = 0.425f;
	CowKeyFrame[12].cowPosFLZ = -0.2f;
	
	CowKeyFrame[13].cowPosX = 6;
	CowKeyFrame[13].cowPosY = 0;
	CowKeyFrame[13].cowPosZ = -6;
	CowKeyFrame[13].cowRotY = -90;
	CowKeyFrame[13].cowRotBLX = 0;
	CowKeyFrame[13].cowRotFLX = 0;
	CowKeyFrame[13].cowRotBLZ = 0;
	CowKeyFrame[13].cowRotFLZ = 0;
	CowKeyFrame[13].cowPosBLX = -0.506f;
	CowKeyFrame[13].cowPosBLZ = -0.2f;
	CowKeyFrame[13].cowPosBRX = -0.59f;
	CowKeyFrame[13].cowPosBRZ = 0.1f;
	CowKeyFrame[13].cowPosFRX = 0.430f;
	CowKeyFrame[13].cowPosFRZ = 0.15f;
	CowKeyFrame[13].cowPosFLX = 0.425f;
	CowKeyFrame[13].cowPosFLZ = -0.2f;
	
	CowKeyFrame[14].cowPosX = 7;
	CowKeyFrame[14].cowPosY = 0;
	CowKeyFrame[14].cowPosZ = -6;
	CowKeyFrame[14].cowRotY = -90;
	CowKeyFrame[14].cowRotBLX = 0;
	CowKeyFrame[14].cowRotFLX = 0;
	CowKeyFrame[14].cowRotBLZ = 15;
	CowKeyFrame[14].cowRotFLZ = -15;
	CowKeyFrame[14].cowPosBLX = -0.506f;
	CowKeyFrame[14].cowPosBLZ = -0.2f;
	CowKeyFrame[14].cowPosBRX = -0.59f;
	CowKeyFrame[14].cowPosBRZ = 0.1f;
	CowKeyFrame[14].cowPosFRX = 0.430f;
	CowKeyFrame[14].cowPosFRZ = 0.15f;
	CowKeyFrame[14].cowPosFLX = 0.425f;
	CowKeyFrame[14].cowPosFLZ = -0.2f;
	
	CowKeyFrame[15].cowPosX = 8;
	CowKeyFrame[15].cowPosY = 0;
	CowKeyFrame[15].cowPosZ = -6;
	CowKeyFrame[15].cowRotY = -90;
	CowKeyFrame[15].cowRotBLX = 0;
	CowKeyFrame[15].cowRotFLX = 0;
	CowKeyFrame[15].cowRotBLZ = 35;
	CowKeyFrame[15].cowRotFLZ = -35;
	CowKeyFrame[15].cowPosBLX = -0.506f;
	CowKeyFrame[15].cowPosBLZ = -0.2f;
	CowKeyFrame[15].cowPosBRX = -0.59f;
	CowKeyFrame[15].cowPosBRZ = 0.1f;
	CowKeyFrame[15].cowPosFRX = 0.430f;
	CowKeyFrame[15].cowPosFRZ = 0.15f;
	CowKeyFrame[15].cowPosFLX = 0.425f;
	CowKeyFrame[15].cowPosFLZ = -0.2f;
	
	CowKeyFrame[16].cowPosX = 9;
	CowKeyFrame[16].cowPosY = 0;
	CowKeyFrame[16].cowPosZ = -6;
	CowKeyFrame[16].cowRotY = -90;
	CowKeyFrame[16].cowRotBLX = 0;
	CowKeyFrame[16].cowRotFLX = 0;
	CowKeyFrame[16].cowRotBLZ = 0;
	CowKeyFrame[16].cowRotFLZ = 0;
	CowKeyFrame[16].cowPosBLX = -0.506f;
	CowKeyFrame[16].cowPosBLZ = -0.2f;
	CowKeyFrame[16].cowPosBRX = -0.59f;
	CowKeyFrame[16].cowPosBRZ = 0.1f;
	CowKeyFrame[16].cowPosFRX = 0.430f;
	CowKeyFrame[16].cowPosFRZ = 0.15f;
	CowKeyFrame[16].cowPosFLX = 0.425f;
	CowKeyFrame[16].cowPosFLZ = -0.2f;
	
	CowKeyFrame[17].cowPosX = 10;
	CowKeyFrame[17].cowPosY = 0;
	CowKeyFrame[17].cowPosZ = -6;
	CowKeyFrame[17].cowRotY = -90;
	CowKeyFrame[17].cowRotBLX = 0;
	CowKeyFrame[17].cowRotFLX = 0;
	CowKeyFrame[17].cowRotBLZ = -15;
	CowKeyFrame[17].cowRotFLZ = 15;
	CowKeyFrame[17].cowPosBLX = -0.506f;
	CowKeyFrame[17].cowPosBLZ = -0.2f;
	CowKeyFrame[17].cowPosBRX = -0.59f;
	CowKeyFrame[17].cowPosBRZ = 0.1f;
	CowKeyFrame[17].cowPosFRX = 0.430f;
	CowKeyFrame[17].cowPosFRZ = 0.15f;
	CowKeyFrame[17].cowPosFLX = 0.425f;
	CowKeyFrame[17].cowPosFLZ = -0.2f;
	
	CowKeyFrame[18].cowPosX = 11;
	CowKeyFrame[18].cowPosY = 0;
	CowKeyFrame[18].cowPosZ = -6;
	CowKeyFrame[18].cowRotY = -90;
	CowKeyFrame[18].cowRotBLX = 0;
	CowKeyFrame[18].cowRotFLX = 0;
	CowKeyFrame[18].cowRotBLZ = -35;
	CowKeyFrame[18].cowRotFLZ = 35;
	CowKeyFrame[18].cowPosBLX = -0.506f;
	CowKeyFrame[18].cowPosBLZ = -0.2f;
	CowKeyFrame[18].cowPosBRX = -0.59f;
	CowKeyFrame[18].cowPosBRZ = 0.1f;
	CowKeyFrame[18].cowPosFRX = 0.430f;
	CowKeyFrame[18].cowPosFRZ = 0.15f;
	CowKeyFrame[18].cowPosFLX = 0.425f;
	CowKeyFrame[18].cowPosFLZ = -0.2f;
	
	CowKeyFrame[19].cowPosX = 12;
	CowKeyFrame[19].cowPosY = 0;
	CowKeyFrame[19].cowPosZ = -6;
	CowKeyFrame[19].cowRotY = -90;
	CowKeyFrame[19].cowRotBLX = 0;
	CowKeyFrame[19].cowRotFLX = 0;
	CowKeyFrame[19].cowRotBLZ = 0;
	CowKeyFrame[19].cowRotFLZ = 0;
	CowKeyFrame[19].cowPosBLX = -0.506f;
	CowKeyFrame[19].cowPosBLZ = -0.2f;
	CowKeyFrame[19].cowPosBRX = -0.59f;
	CowKeyFrame[19].cowPosBRZ = 0.1f;
	CowKeyFrame[19].cowPosFRX = 0.430f;
	CowKeyFrame[19].cowPosFRZ = 0.15f;
	CowKeyFrame[19].cowPosFLX = 0.425f;
	CowKeyFrame[19].cowPosFLZ = -0.2f;
	
	CowKeyFrame[20].cowPosX = 12;
	CowKeyFrame[20].cowPosY = 0;
	CowKeyFrame[20].cowPosZ = -6;
	CowKeyFrame[20].cowRotY = -180;
	CowKeyFrame[20].cowRotBLX = 0;
	CowKeyFrame[20].cowRotFLX = 0;
	CowKeyFrame[20].cowRotBLZ = 0;
	CowKeyFrame[20].cowRotFLZ = 0;
	CowKeyFrame[20].cowPosBLX = 0.2f;
	CowKeyFrame[20].cowPosBLZ = -0.506f;
	CowKeyFrame[20].cowPosBRX = -0.1f;
	CowKeyFrame[20].cowPosBRZ = -0.59f;
	CowKeyFrame[20].cowPosFRX = -0.15f;
	CowKeyFrame[20].cowPosFRZ = 0.430f;
	CowKeyFrame[20].cowPosFLX = 0.2f;
	CowKeyFrame[20].cowPosFLZ = 0.425f;
	
	CowKeyFrame[21].cowPosX = 12;
	CowKeyFrame[21].cowPosY = 0;
	CowKeyFrame[21].cowPosZ = -5;
	CowKeyFrame[21].cowRotY = -180;
	CowKeyFrame[21].cowRotBLX = 15;
	CowKeyFrame[21].cowRotFLX = -15;
	CowKeyFrame[21].cowRotBLZ = 0;
	CowKeyFrame[21].cowRotFLZ = 0;
	CowKeyFrame[21].cowPosBLX = 0.2f;
	CowKeyFrame[21].cowPosBLZ = -0.506f;
	CowKeyFrame[21].cowPosBRX = -0.1f;
	CowKeyFrame[21].cowPosBRZ = -0.59f;
	CowKeyFrame[21].cowPosFRX = -0.15f;
	CowKeyFrame[21].cowPosFRZ = 0.430f;
	CowKeyFrame[21].cowPosFLX = 0.2f;
	CowKeyFrame[21].cowPosFLZ = 0.425f;
	
	CowKeyFrame[22].cowPosX = 12;
	CowKeyFrame[22].cowPosY = 0;
	CowKeyFrame[22].cowPosZ = -4;
	CowKeyFrame[22].cowRotY = -180;
	CowKeyFrame[22].cowRotBLX = 35;
	CowKeyFrame[22].cowRotFLX = -35;
	CowKeyFrame[22].cowRotBLZ = 0;
	CowKeyFrame[22].cowRotFLZ = 0;
	CowKeyFrame[22].cowPosBLX = 0.2f;
	CowKeyFrame[22].cowPosBLZ = -0.506f;
	CowKeyFrame[22].cowPosBRX = -0.1f;
	CowKeyFrame[22].cowPosBRZ = -0.59f;
	CowKeyFrame[22].cowPosFRX = -0.15f;
	CowKeyFrame[22].cowPosFRZ = 0.430f;
	CowKeyFrame[22].cowPosFLX = 0.2f;
	CowKeyFrame[22].cowPosFLZ = 0.425f;
	
	CowKeyFrame[23].cowPosX = 12;
	CowKeyFrame[23].cowPosY = 0;
	CowKeyFrame[23].cowPosZ = -3;
	CowKeyFrame[23].cowRotY = -180;
	CowKeyFrame[23].cowRotBLX = 0;
	CowKeyFrame[23].cowRotFLX = 0;
	CowKeyFrame[23].cowRotBLZ = 0;
	CowKeyFrame[23].cowRotFLZ = 0;
	CowKeyFrame[23].cowPosBLX = 0.2f;
	CowKeyFrame[23].cowPosBLZ = -0.506f;
	CowKeyFrame[23].cowPosBRX = -0.1f;
	CowKeyFrame[23].cowPosBRZ = -0.59f;
	CowKeyFrame[23].cowPosFRX = -0.15f;
	CowKeyFrame[23].cowPosFRZ = 0.430f;
	CowKeyFrame[23].cowPosFLX = 0.2f;
	CowKeyFrame[23].cowPosFLZ = 0.425f;



	//Inicialización de KeyFrames
	//for (int i = 0; i < MAX_FRAMES; i++)
	//{
	//	KeyFrame[i].cowPosX = 0;
	//	KeyFrame[i].cowPosY = 0;
	//	KeyFrame[i].cowPosZ = 0;
	//	KeyFrame[i].cowRotY = 0;
	//	KeyFrame[i].cowBl = 0;
	//	KeyFrame[i].cowFl = 0;

	//}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{	
		skyboxShader.setInt("skybox", 0);
		
		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.5f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Starts stage
		// -------------------------------------------------------------------------------------------------------------------------
		
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		// Ground
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		ground.Draw(staticShader);
		
		//Indian camp 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(42.0f, -0.2f, 42.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tipi.Draw(staticShader);
		
		//Windmill
		model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, -0.2f, 50.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		windmill.Draw(staticShader);

		//WaterTower
		model = glm::translate(glm::mat4(1.0f), glm::vec3(43.0f, -0.2f, 50.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		waterTower.Draw(staticShader);

		//Barrel 1 -- windmill
		model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, -0.2f, 42.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		staticShader.setMat4("model", model);
		barrel1.Draw(staticShader);


		//Cactus 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 41.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 50.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 47.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 5 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, 43.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Tree 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f, -0.2f, 45.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree1.Draw(staticShader);

		//Tree 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(17.0f, -0.2f, 55.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree2.Draw(staticShader);

		//Tree 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.2f, 50.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree2.Draw(staticShader);

		//Tree 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.2f, 43.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree1.Draw(staticShader);

		//Tree 5 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, -0.2f, 48.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree2.Draw(staticShader);

		//Tree 6 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -0.2f, 54.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		tree1.Draw(staticShader);
		
		
		// Rock 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, -0.2f, 42.0f));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		// Rock 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, -0.2f, 50.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		// Rock 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 46.0f));
		model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		// Rock 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, -0.2f, 55.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.065f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		// Wood Fence -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-18.0f, -0.2f, 43.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		woodFence.Draw(staticShader);

		//Barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-17.0f, -0.2f, 40.0f));
		model = glm::scale(model, glm::vec3(0.55f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		barn.Draw(staticShader);

		// Barn -- cow 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-31.5f, -0.2f, 47.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cow.Draw(staticShader);

		// Barn -- cow 2
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, -0.2f, 49.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cow.Draw(staticShader);

		// Barn -- bull 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-38.0f, -0.2f, 50.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bull.Draw(staticShader);

		// Barn -- bull 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-41.0f, -0.2f,  48.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bull.Draw(staticShader);

		//Cactus 5 -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-31.0f, -0.2f, 40.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 6 -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, -0.2f, 42.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);
    
		// Bank
		model = glm::translate(glm::mat4(1.0f), glm::vec3(67.0f, 0.0f, -22.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		bank.Draw(staticShader);

		// Store
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-53.0f, 0.0f, -23.0f));
		model = glm::scale(model, glm::vec3(23.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		store.Draw(staticShader);

		// Church
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-46.0f, 0.5f, -78.0f));
		model = glm::scale(model, glm::vec3(34.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		church.Draw(staticShader);

		// Gallow
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-12.0f, 1.5f, -34.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		gallow.Draw(staticShader);

		// House 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(45.0f, 0.2f, 27.0f));
		model = glm::scale(model, glm::vec3(25.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		house.Draw(staticShader);

		// House 2
		model = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 0.2f, 67.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		house2.Draw(staticShader);

		// House 3
		model = glm::translate(glm::mat4(1.0f), glm::vec3(75.0f, 0.2f, 36.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		house3.Draw(staticShader);

		// House 4
		model = glm::translate(glm::mat4(1.0f), glm::vec3(11.5f, 0.2f, 5.0f));
		model = glm::scale(model, glm::vec3(20.0f));
		staticShader.setMat4("model", model);
		house4.Draw(staticShader);

		// House 5
		model = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 0.2f, 4.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::rotate(model, glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		house5.Draw(staticShader);

		// BAR
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-18.0f,0.0f,12.0f));
		model = glm::scale(model,glm::vec3(0.6f));
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		saloon.Draw(staticShader);

		// SHERIFF OFFICE
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, 28.0f));
		model = glm::scale(model, glm::vec3(0.045f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		sheriff_office.Draw(staticShader);

		// HOTEL
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.05f));
		model = glm::translate(model, glm::vec3(-530.0f, 70.0f, -255.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		hotel.Draw(staticShader);


		// ESTACIÓN DE TREN
		model = glm::mat4(4.0f);
		model = glm::translate(model, glm::vec3(-45.0f,-0.5f,-10.0f));
		model = glm::scale(model, glm::vec3(0.038f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		train_station.Draw(staticShader);

		// Vias del Tren
		model = glm::mat4(4.0f);
		model = glm::translate(model, glm::vec3(-60.0f, -0.3f, -17.0f));
		model = glm::scale(model, glm::vec3(0.020f));
		tmp = model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(1265.0f,0.0f,0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		model = glm::translate(model, glm::vec3(1265.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1265.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-1265.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		model = glm::translate(model, glm::vec3(-1265.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);
		
		model = glm::translate(model, glm::vec3(-1265.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		rail.Draw(staticShader);

		// Doctor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		staticShader.setMat4("model",model);
		doctor.Draw(staticShader);

		// Túneles
		model = glm::mat4(1.0f);
		model = glm::translate(model,glm::vec3(-60.0f, -1.0f, 80.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tunnel.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-60.0f, -1.0f, -70.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tunnel.Draw(staticShader);

		// Montañas
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -126.0f));
		staticShader.setMat4("model", model);
		mountain.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 138.0f));
		staticShader.setMat4("model", model);
		mountain.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// End of stage
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Start animations
		// -------------------------------------------------------------------------------------------------------------------------
		
		// --------Walking cow-----------
		// Body of the cow
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-30.0f+cowPosX, -0.25f+cowPosY, 40.0f+cowPosZ));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(cowRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cowBody.Draw(staticShader);

		// Back right leg of the cow
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-30.0f + cowPosX + cowPosBRX, 0.47f + cowPosY, 40.0f + cowPosZ + cowPosBRZ));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(cowRotBLX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cowRotBLZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(cowRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cowBackRightLeg.Draw(staticShader);

		// Back left leg of the cow
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-30.0f + cowPosX + cowPosBLX, 0.47f + cowPosY, 40.0f + cowPosZ + cowPosBLZ));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(cowRotFLX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cowRotFLZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(cowRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cowBackLeftLeg.Draw(staticShader);

		// Front right leg of the cow
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-30.0f + cowPosX + cowPosFRX, 0.265f + cowPosY, 40.0f + cowPosZ + cowPosFRZ));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(cowRotBLX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cowRotBLZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(cowRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cowFrontRightLeg.Draw(staticShader);

		// Front left leg of the cow
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-30.0f + cowPosX + cowPosFLX, 0.33f + cowPosY, 40.0f + cowPosZ + cowPosFLZ));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(cowRotFLX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cowRotFLZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(cowRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cowFrontLeftLeg.Draw(staticShader);

		// --------Shooter tumbleweed-----------
		// Body
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(tumblePosX, tumblePosY, tumblePosZ));
		model = glm::scale(model, glm::vec3(0.6f));
		model = glm::rotate(model, glm::radians(tumbleRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(tumbleRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		tumbleWeed.Draw(staticShader);

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//Cow Movement
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		cowPosZ--;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		cowPosZ++;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		cowPosX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cowPosX++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		cowPosY++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		cowPosY--;

	//Tumbleweed Animation
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		if (tumbleState == 0) {
			playTumbleWeed = true,
			tumbleUp = false;
			tumblePosX = -9.0f,
			tumblePosY = -0.1f,
			tumblePosZ = 50.0f,
			tumbleRotY = -180.0f,
			tumbleRotX = 0.0f;
			tumbleState = 1,
			tumbleShootCount = 0;
		}

	//To play KeyFrame animation 
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		std::cout << CowFrameIndex << std::endl;
		if (cowPlay == false && (CowFrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			bool played = PlaySound(L"cow.wav", NULL, SND_ASYNC);
			cout << "Ambient:" << played << endl;
			resetElements();
			//First Interpolation				
			interpolation();

			cowPlay = true;
			cowPlayIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			cowPlay = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (CowFrameIndex < COW_MAX_FRAMES)
		{
			saveFrame();
			std::cout << "Frame saved" << std::endl;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
	camera.ProcessMouseScroll(yoffset);
}