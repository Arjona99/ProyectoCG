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

// posiciones
//float x = 0.0f;
//float y = 0.0f;
float	movAuto_x = 0.0f,
		movAuto_z = 0.0f,
		orienta = 0.0f;
bool	animacion = false,
		recorrido1 = true,
		recorrido2 = false,
		recorrido3 = false,
		recorrido4 = false;

//Animación Buitre
float	posX_buitre = 0.0f,
		posY_buitre = 0.0f,
		giro_alas = -60.0f,
		giro_buitre = 90.0f,
		sentido_giro_b = 1.0f;
int		estado_buitre = 0,
		contador_cuadros = 0;

//Animación Tren
float	giro_llantas = 0.0f,
		posX_barra = 0.0f,
		posX_tren  = 0.0f,
		posY_barra = 0.0f,
		posY_barra_f = 0.0f,
		giro_barra = 0.0f,
		giro_barra_f   = 0.0f,
		sentidoX_barra = 1.0f,
		sentidoY_barra = -1.0f,
		posX_barrera = 0.0f;
int		estado_barra = 0;

boolean animacion_tren  = false,
		parada_estacion = false;

//Animacion Villager
float	giro_piernas = 0.0f,
		sentido_piernas = 1.0f,
		posX_villager = 0.0f,
		posZ_villager = 0.0f,
		giroVillager = 0.0f,
		sentidoVillager = 1.0f;

int cuadrosVillager = 0;
int estadosVillager = 0;

//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
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
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	//====================== Animacion Alas Buitre ============================
	if (giro_alas >= -60.0f && giro_alas <= 60.0f) {
		giro_alas += 2.0f * (sentido_giro_b);

		if (giro_alas == -60.0f || giro_alas == 60.0f) {
			sentido_giro_b = sentido_giro_b * -1.0f;
		}
	}

	/*cout << posX_buitre << endl;
	cout << posY_buitre << endl;
	cout << estado_buitre << endl;
	//cout << "---------" << endl;*/

	//====================== ANIMACION GIRO BUITRE =============================
	switch (estado_buitre) {
		case 0:
			giro_buitre = 90.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre += 0.05f;
			if (posX_buitre >= 4.0f) {
				
				estado_buitre = 1;
			}
			break;
		case 1:
			giro_buitre = 45.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre += 0.05f;
			posY_buitre += 0.05f;
			if (posX_buitre >= 8.0f && posY_buitre >= 4.0f) {
				
				
				estado_buitre = 2;
			}
			break;
		case 2:
			giro_buitre = 0.0f;
			//giro_buitre -= 0.5625f;
			//posX_buitre += 0.2f;
			posY_buitre += 0.05f;
			if (posY_buitre >= 8.0f) {
				
				estado_buitre = 3;
			}
			break;
		case 3:
			giro_buitre = -45.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre -= 0.05f;
			posY_buitre += 0.05f;
			if (posY_buitre >= 12.0f && posX_buitre <= 4.0f) {
				
				estado_buitre = 4;
			}
			break;
		case 4:
			giro_buitre = -90.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre -= 0.05f;
			//posY_buitre += 0.2f;
			if (posX_buitre <= 0.0f) {
				
				estado_buitre = 5;
			}
			break;
		case 5:
			giro_buitre = 225.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre -= 0.05f;
			posY_buitre -= 0.05f;
			if (posY_buitre <= 8.0f && posX_buitre <= -4.0f) {
				
				
				estado_buitre = 6;
			}
			break;
		case 6:
			giro_buitre = 180.0f;
			//giro_buitre -= 0.5625f;
			//posX_buitre -= 0.2f;
			posY_buitre -= 0.05f;
			if (posY_buitre <= 4.0f) {
				
				estado_buitre = 7;
			}
			break;
		case 7:
			giro_buitre = 135.0f;
			//giro_buitre -= 0.5625f;
			posX_buitre += 0.05f;
			posY_buitre -= 0.05f;
			if (posY_buitre <= 0.0f && posX_buitre >= 0.0f) {
				
				
				estado_buitre = 0;
			}
			break;
		default:
			//cout << "Default" << endl;
			estado_buitre = 0;
			break;
	}


	//====================== ANIMACION TREN =====================================
	if (animacion_tren) {
		giro_llantas -= 3.0f;
		//posX_barra += 0.02f * sentidoX_barra;
		//posY_barra += 0.02f * sentidoY_barra;

	
		//Animacion barra trasera
		switch (estado_barra) {
			case 0:
			case 10:
			case 11:
				posX_barra += 0.02f;
				posY_barra -= 0.02f;
				break;
			case 1:
			case 2:
			case 3:
				posX_barra += 0.02f;
				posY_barra += 0.02f;
				break;
			case 4:
			case 5:
			case 6:
				posX_barra -= 0.02f;
				posY_barra += 0.02f;
				break;
			case 7:
			case 8:
			case 9:
				posX_barra -= 0.02f;
				posY_barra -= 0.02f;
				break;
			default:
				cout << "DEFAULT" << endl;
				break;

		}

		//Animacion Barra delantera
		switch (estado_barra) {
			case 0:
			case 10:
				posY_barra_f += 0.03f;
				giro_barra_f += 0.36f;
				break;
			case 1:
				posY_barra_f += 0.03f;
				giro_barra_f -= 0.36f;
				break;
			case 2:
				giro_barra_f -= 0.21f;
				break;
			case 3:
				posY_barra_f += 0.03f;
				giro_barra_f -= 0.36f;
				break;
			case 4:
			case 6:
				posY_barra_f -= 0.03f;
				giro_barra_f -= 0.21f;
				break;
			case 5:
				giro_barra_f -= 0.36f;
				break;
			case 7:
				posY_barra_f -= 0.03f;
				giro_barra_f += 0.21f;
				break;
			case 8:
				giro_barra_f += 0.36f;
				break;
			case 9:
				posY_barra_f -= 0.03f;
				giro_barra_f += 0.36f;
				break;
			case 11:
				giro_barra_f += 0.06f;
				break;
			default:
				cout << "DEFAULT" << endl;
				break;
		}

		contador_cuadros++;
		if (contador_cuadros == 10) {
			estado_barra++;
			contador_cuadros = 0;
		}
		if (estado_barra == 12)
			estado_barra = 0;
	
	
		posX_tren += 0.1f;
		if (posX_tren >= 80.0f && !parada_estacion) {
			animacion_tren = false;
			parada_estacion = true;
		}
	}
	

	//====================== ANIMACION VILLAGER =================================
	if (giro_piernas >= -30.0f && giro_piernas <= 30.0f) {
		giro_piernas += 1.5f * (sentido_piernas);

		if (giro_piernas == -30.0f || giro_piernas == 30.0f) {
			sentido_piernas *= -1.0f;
		}
	}

	switch (estadosVillager) {
		case 0:
			
			posX_villager += 0.427f * sentidoVillager;
			if (sentidoVillager < 0) {
				giroVillager = 180.0f;
			}
			else {
				giroVillager = 0.0f;
			}
			break;
		case 1:
			posZ_villager -= 0.595 * sentidoVillager;
			giroVillager = 90.0f * sentidoVillager;
			break;
		case 2:
			posX_villager += 0.363f * sentidoVillager;
			if (sentidoVillager < 0) {
				giroVillager = 180.0f;
			}
			else {
				giroVillager = 0.0f;
			}
			break;
		
		default:
			//cout << "DEFAULT" << endl;
			//estadosVillager = 0;
			break;
	}

	cuadrosVillager++;
	if (cuadrosVillager == 100) {
		cuadrosVillager = 0;
		estadosVillager+= 1 * (int)sentidoVillager;
		if (estadosVillager == 3 || estadosVillager == -1) {
			sentidoVillager *= -1.0f;
		}
	}

	

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
	Model tree3("resources/objects/tree3/Gledista_Triacanthos_3.obj");
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
	Model mountain("resources/objects/mountain/mountain.obj");


	Model vulture_torso("resources/objects/vulture_torso/vulture_torso.obj");
	Model vulture_left_wing("resources/objects/vulture_left_wing/vulture_left_wing.obj");
	Model vulture_right_wing("resources/objects/vulture_right_wing/vulture_right_wing.obj");

	//MODELO TREN POR PIEZAS
	Model loco("resources/objects/train/loco/loco.obj");

	Model barraDerechaBack("resources/objects/train/barra_derecha_back/barra_derecha_back.obj");
	Model barraIzquierdaBack("resources/objects/train/barra_izquierda_back/barra_izquierda_back.obj");

	Model barraDerechaFront("resources/objects/train/barra_derecha_front/barra_derecha_front.obj");
	Model barraIzquierdaFront("resources/objects/train/barra_izquierda_front/barra_izquierda_front.obj");

	Model barreraDerecha("resources/objects/train/barrera_derecha/barrera_derecha.obj");
	Model barreraIzquierda("resources/objects/train/barrera_izquierda/barrera_izquierda.obj");

	Model leftBackBigWheel("resources/objects/train/left_back_big_wheel/left_back_big_wheel.obj");
	Model leftBackSmallWheel("resources/objects/train/left_back_small_wheel/left_back_small_wheel.obj");
	Model leftFrontBigWheel("resources/objects/train/left_front_big_wheel/left_front_big_wheel.obj");
	Model leftFrontSmallWheel("resources/objects/train/left_front_small_wheel/left_front_small_wheel.obj");

	Model rightBackBigWheel("resources/objects/train/right_back_big_wheel/right_back_big_wheel.obj");
	Model rightBackSmallWheel("resources/objects/train/right_back_small_wheel/right_back_small_wheel.obj");
	Model rightFrontBigWheel("resources/objects/train/right_front_big_wheel/right_front_big_wheel.obj");
	Model rightFrontSmallWheel("resources/objects/train/right_front_small_wheel/right_front_small_wheel.obj");

	Model tuercaRightFront("resources/objects/train/tuerca_front_right/tuerca_front_right.obj");
	Model tuercaLeftFront("resources/objects/train/tuerca_left_front/tuerca_left_front.obj");
	Model tuercaRightBack("resources/objects/train/tuerca_right_back/tuerca_right_back.obj");
	Model tuercaLeftBack("resources/objects/train/tuerca_left_back/tuerca_left_back.obj");

	//ADEANOS

	Model minecraftVillager("resources/objects/minecraftVillager/minecraftVillager.obj");

	Model minecraftVillagerTorso("resources/objects/minecraftVillagerTorso/minecraftVillagerTorso.obj");
	Model minecraftVillagerRightLeg("resources/objects/minecraftVillagerRightLeg/minecraftVillagerRightLeg.obj");
	Model minecraftVillagerLeftLeg("resources/objects/minecraftVillagerLeftLeg/minecraftVillagerLeftLeg.obj");

	Model trainWagon("resources/objects/trainWagon/trainWagon.obj");

	



	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

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
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
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
		// Escenario
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
		tree3.Draw(staticShader);

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
		tree3.Draw(staticShader);
		
		
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
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, -0.2f, 40.0f));
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



			// -------------------------------------------------------------------------------------------------------------------------
			// BAR
			// -------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-18.0f,0.0f,12.0f));
		model = glm::scale(model,glm::vec3(0.6f));
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		saloon.Draw(staticShader);

			// --------------------------------------------------------------------------------------------------------------------------
			// SHERIFF OFFICE
			// --------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, 28.0f));
		model = glm::scale(model, glm::vec3(0.045f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		sheriff_office.Draw(staticShader);

			// ---------------------------------------------------------------------------------------------------------------------------
			// HOTEL
			// ---------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.05f));
		model = glm::translate(model, glm::vec3(-530.0f, 70.0f, -255.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		hotel.Draw(staticShader);

			// ----------------------------------------------------------------------------------------------------------------------------
			// ESTACIÓN DE TREN
			// ----------------------------------------------------------------------------------------------------------------------------
		model = glm::mat4(4.0f);
		model = glm::translate(model, glm::vec3(-45.0f,-0.5f,-10.0f));
		model = glm::scale(model, glm::vec3(0.038f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		train_station.Draw(staticShader);

			// -----------------------------------------------------------------------------------------------------------------------------
			// Vias del Tren
			// -----------------------------------------------------------------------------------------------------------------------------

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

			// -------------------------------------------------------------------------------------------------------------------------
			// Doctor
			// -------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.15f));
		staticShader.setMat4("model",model);
		doctor.Draw(staticShader);
		
			// -------------------------------------------------------------------------------------------------------------------------
			// Túneles
			// -------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model,glm::vec3(-60.0f, -1.0f, 80.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tunnel.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-60.0f, -1.0f, -70.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tunnel.Draw(staticShader);

			// -------------------------------------------------------------------------------------------------------------------------
			// Montañas
			// -------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -126.0f));
		staticShader.setMat4("model", model);
		mountain.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 138.0f));
		staticShader.setMat4("model", model);
		mountain.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//Buitre
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(0.0f + posX_buitre, 40.0f, 0.0f + posY_buitre));
		model = glm::rotate(model, glm::radians(giro_buitre), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		vulture_torso.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_left_wing.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_right_wing.Draw(staticShader);

		//~~~~~~~~~~~~ Buitre 2 ~~~~~~~~~~~~
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(5.0f - posX_buitre, 50.0f, 12.0f - posY_buitre));
		model = glm::rotate(model, glm::radians(giro_buitre+180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		vulture_torso.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre+180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_left_wing.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre+180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_right_wing.Draw(staticShader);

		//~~~~~~~~~~~~ Buitre 3 ~~~~~~~~~~~~
		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(-10.0f - posX_buitre, 45.0f, -20.0f - posY_buitre));
		model = glm::rotate(model, glm::radians(giro_buitre + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		vulture_torso.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_left_wing.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_buitre + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-giro_alas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		vulture_right_wing.Draw(staticShader);

		// ======================== Wagon ============================

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-60.0f, -0.5f, 105.0f - posX_tren));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		trainWagon.Draw(staticShader);


		// ======================== TREN =============================

		model = glm::mat4(1.0f);
		tmp = model = glm::translate(model, glm::vec3(-60.0f, -0.5f, 90.0f - posX_tren));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		loco.Draw(staticShader);


		//---------------Llantas-----------------
		model = glm::translate(tmp, glm::vec3(1.4f, 2.0f, -5.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		rightFrontBigWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(1.4f, 2.0f, 1.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		rightBackBigWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(1.6f, 1.0f, -12.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		rightFrontSmallWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(1.6f, 1.0f, -9.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		rightBackSmallWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.4f, 2.0f, -5.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		leftFrontBigWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.4f, 2.0f, 1.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		leftBackBigWheel.Draw(staticShader);


		model = glm::translate(tmp, glm::vec3(-1.6f, 1.0f, -12.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		leftFrontSmallWheel.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.6f, 1.0f, -9.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		leftBackSmallWheel.Draw(staticShader);

		//--------------Tuercas-----------------
		model = glm::translate(tmp, glm::vec3(1.6f, 2.0f, -5.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		tuercaRightFront.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(1.6f, 2.0f, 1.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		tuercaRightBack.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.6f, 2.0f, -5.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		tuercaLeftFront.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.6f, 2.0f, 1.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		tuercaLeftBack.Draw(staticShader);

		//----------------Barras-------------------

		model = glm::translate(tmp, glm::vec3(1.7f, 1.6f + posY_barra, 0.6f + posX_barra));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f));
		staticShader.setMat4("model", model);
		barraDerechaBack.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(2.0f, 2.1f, -9.1f + posY_barra_f));
		model = glm::rotate(model, glm::radians(giro_barra_f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		barraDerechaFront.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-1.7f, 1.6f + posY_barra, 0.6f + posX_barra));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f));
		staticShader.setMat4("model", model);
		barraIzquierdaBack.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-2.0f, 2.1f, -9.1f + posY_barra_f));
		model = glm::rotate(model, glm::radians(giro_barra_f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		barraIzquierdaFront.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-2.0f, 2.1f, -9.1f + posY_barra_f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		barreraIzquierda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(2.0f, 2.1f, -9.1f+ posY_barra_f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		barreraDerecha.Draw(staticShader);

		// ================================ Villagers ====================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, 8.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 18.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 31.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 31.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -27.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, -22.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, -27.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillager.Draw(staticShader);


		//------Animados-------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-35.0f + posX_villager, 1.0f, 35.0f+posZ_villager));
		tmp = model = glm::rotate(model, glm::radians(90.0f+giroVillager), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillagerTorso.Draw(staticShader);

		
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giro_piernas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillagerLeftLeg.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-giro_piernas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		minecraftVillagerRightLeg.Draw(staticShader);

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
	//Animacion TREN
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ_villager += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ_villager -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX_villager -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX_villager += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		cout << "PosX: " << posX_villager << endl;
		cout << "PosZ: " << posZ_villager << endl;
		cout << "-----------------------------" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		giro_llantas++;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		animacion_tren = animacion_tren ^ true;
	}


	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
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