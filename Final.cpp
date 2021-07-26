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
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
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

	//Vehículo
	if (animacion)
	{
		movAuto_z += 3.0f;
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
	Model superficie("resources/objects/piso/superficie.obj");
	Model tree1("resources/objects/tree1/Gledista_Triacanthos.obj");
	Model tree2("resources/objects/tree2/Gledista_Triacanthos_2.obj");
	Model tree3("resources/objects/tree3/Gledista_Triacanthos_3.obj");
	Model windmill("resources/objects/windmill/windmill.obj");
	Model cactus("resources/objects/cactus/cactus.obj");
	Model tipi("resources/objects/tipi/tipi.obj");
	Model cow("resources/objects/cow/cow.obj");
	Model bull("resources/objects/bull/bull.obj");


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
		//model = glm::translate(model, glm::vec3(0.0f, -1.75, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		superficie.Draw(staticShader);
		
		//Indian camp 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 0.0f, 230.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tipi.Draw(staticShader);
		
		//Windmill
		model = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 0.0f, 250.0f));
		model = glm::scale(model, glm::vec3(3.5f));
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		windmill.Draw(staticShader);

		//Barrel 1 -- windmill
		model = glm::translate(glm::mat4(1.0f), glm::vec3(115.0f, 0.0f, 242.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		barrel1.Draw(staticShader);


		//Cactus 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-55.0f, 0.0f, 250.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 0.0f, 230.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 0.0f, 280.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 263.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 5 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(29.0f, 0.0f, 291.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Tree 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-75.0f, 0.0f, 260.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree1.Draw(staticShader);

		//Tree 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, 0.0f, 290.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree2.Draw(staticShader);

		//Tree 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(55.0f, 0.0f, 270.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree3.Draw(staticShader);

		//Tree 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 250.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree1.Draw(staticShader);

		//Tree 5 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 240.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree2.Draw(staticShader);

		//Tree 6 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(115.0f, 0.0f, 280.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		tree3.Draw(staticShader);
		
		//Rock 1 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 256.0f));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		//Rock 2 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-70.0f, 0.0f, 235.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		//Rock 3 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(90.0f, 0.0f, 285.0f));
		model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		//Rock 4 -- park
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.0f, 0.0f, 265.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		rock1.Draw(staticShader);

		// Wood Fence -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-110.0f, 0.0f, 220.0f));
		model = glm::scale(model, glm::vec3(2.3f));
		staticShader.setMat4("model", model);
		woodFence.Draw(staticShader);

		//Barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 200.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		barn.Draw(staticShader);

		// Barn -- cow 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-210.0f, 0.0f, 240.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cow.Draw(staticShader);

		// Barn -- cow 2
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-225.0f, 0.0f, 240.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cow.Draw(staticShader);

		// Barn -- bull 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-255.0f, 0.0f, 240.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bull.Draw(staticShader);

		// Barn -- bull 1
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-225.0f, 0.0f, 230.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bull.Draw(staticShader);

		//Cactus 5 -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 0.0f, 200.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		//Cactus 6 -- barn
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, 0.0f, 280.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		staticShader.setMat4("model", model);
		cactus.Draw(staticShader);

		

		

		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

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
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
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