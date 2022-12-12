// Code adapted from www.learnopengl.com, www.glfw.org

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/constants.hpp>

#include "shader.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

// Window dimensions
const GLuint WIDTH = 640, HEIGHT = 640;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];

// Light attributes
glm::vec3 lightPos(1.0f, 0.0f, 1.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


//Instantiated Objects Set up
int objectsInstantiated = 0;
GLfloat timeLastPressed = 0;
GLfloat pressDelayTime = 0.5f;
int itteration = 0;

//Destroyable Objects Set up
int destroyableObjects = 0;
GLfloat desZPos = 0;
glm::vec3 destroyableObjectPos[3];

GLfloat timeSinceDesRefresh = 0;
GLfloat desRefreshTime = 1.5f;
bool finishedSpawn = false;


void InstantiatedObject(glm::mat4 view, glm::mat4 projection, GLint modelLoc, GLint viewLoc, GLint projLoc, GLuint VAO) {
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -(GLfloat)glfwGetTime() + timeLastPressed));

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// draw object
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
}


void InstantiatedObjects(glm::mat4 models[100], glm::mat4 view, glm::mat4 projection, GLint modelLoc, GLint viewLoc, GLint projLoc, GLuint VAO, GLfloat distanceTraveled[100]) {
	for (int i = 0; i <= objectsInstantiated; i++)		//For every object instantiated, dray arrays
	{
		if (objectsInstantiated >= i) {		//For each object
			models[i] = glm::scale(models[i], glm::vec3(1.0f, 1.0f, 1.0f));
			//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			models[i] = glm::translate(models[i], glm::vec3(cameraPos.x , cameraPos.y , -distanceTraveled[i]));			//show object at position instantiated

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			// Pass them to the shaders
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// draw object
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
		}
	}
}

void InstantiatedBullet(glm::mat4 models[100], glm::mat4 view, glm::mat4 projection, GLint modelLoc, GLint viewLoc, GLint projLoc, GLuint VAO, GLfloat distanceTraveled[100]) {
	for (int i = 0; i <= objectsInstantiated; i++)		//For every object instantiated, dray arrays
	{
		if (objectsInstantiated >= i) {		//For each object
			models[i] = glm::scale(models[i], glm::vec3(1.0f, 1.0f, 1.0f));
			//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			models[i] = glm::translate(models[i], glm::vec3(cameraPos.x -2.5f + cameraPos.x, cameraPos.y -2.5f , -cameraPos.z + (-distanceTraveled[i] -(GLfloat)glfwGetTime()) + distanceTraveled[i] *3 - distanceTraveled[i]));			//shoot bullet from player perspective
			//models[i] = glm::translate(models[i], glm::vec3(cameraPos.x -2.5f + cameraPos.x, cameraPos.y -2.5f ,  - distanceTraveled[i]));			//shoot bullet from player perspective

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			// Pass them to the shaders
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// draw object
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
		}
	}
}

void InstantiatedDestroyableObject(glm::mat4 models[100], glm::mat4 view, glm::mat4 projection, GLint modelLoc, GLint viewLoc, GLint projLoc, GLuint VAO, GLfloat distanceTraveled[100]) {
	for (int i = 0; i <= destroyableObjects; i++)		//For every object instantiated, dray arrays
	{
		if (destroyableObjects >= i) {		//For each object
			models[i] = glm::scale(models[i], glm::vec3(1.0f, 1.0f, 1.0f));
			//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			models[i] = glm::translate(models[i], glm::vec3(destroyableObjectPos[i].x, 0.0f, -distanceTraveled[i]));			//shoot bullet from player perspective

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			// Pass them to the shaders
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// draw object
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
		}
	}
}



int main(void)
{
	//++++create a glfw window+++++++++++++++++++++++++++++++++++++++
	GLFWwindow* window;

	GLfloat time;

	if (!glfwInit()) //Initialize the library
		return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "Simon's OpenGL Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);//Make the window's context current
								   
	glfwSetKeyCallback(window, key_callback);// Set the required callback functions

	glfwSetCursorPosCallback(window, mouse_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	//++++Initialize GLEW to setup the OpenGL Function pointers+++++++
	glewExperimental = GL_TRUE;
	glewInit();

	//++++Define the viewport dimensions++++++++++++++++++++++++++++
	glViewport(0, 0, HEIGHT, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	//++++++++++Set up vertex data (and buffer(s)) and attribute pointers+++++++++
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f
	};

	GLfloat pyramid[] = {
		2.0f, 2.0f, 2.0f,  1.0f, 0.0f, 0.0f,	//Bottom Left
		3.0f, 2.0f, 2.0f,  1.0f, 0.0f, 0.0f,	//Bottom right
		3.0f,  2.0f, 1.0f,  1.0f, 0.0f, 0.0f,	//Z top Right
		2.0f,  2.0f, 2.0f, 1.0f, 0.0f, 0.0f,	//Bottom left
		2.0f,  2.0f, 1.0f,  1.0f, 0.0f, 0.0f,	//Z top left
		3.0f, 2.0f, 1.0f,  1.0f, 0.0f, 0.0f,	//Z top Right

		2.0f, 2.0f, 2.0f,  0.0f, 1.0f, 0.0f,	//Bottom Left
		3.0f, 2.0f, 2.0f,  0.0f, 1.0f, 0.0f,	//Bottom right
		2.5f,  3.5f, 1.5f,  0.0f, 1.0f, 0.0f,	//top

		2.0f, 2.0f, 1.0f,  0.0f, 0.0f, 1.0f,	//Z top Left
		3.0f, 2.0f, 1.0f,  0.0f, 0.0f, 1.0f,	//Z top right
		2.5f,  3.5f, 1.5f,  0.0f, 0.0f, 1.0f,	//top

		2.0f, 2.0f, 2.0f,  1.0f, 1.0f, 0.0f,	//Bottom Left
		2.0f, 2.0f, 1.0f,  1.0f, 1.0f, 0.0f,	//Z top left
		2.5f,  3.5f, 1.5f,  1.0f, 1.0f, 0.0f,	//top

		3.0f, 2.0f, 2.0f,  1.0f, 0.0f, 1.0f,	//Bottom Right
		3.0f, 2.0f, 1.0f,  1.0f, 0.0f, 1.0f,	//Z top right
		2.5f,  3.5f, 1.5f,  1.0f, 0.0f, 1.0f,	//top
	};

	GLuint VBO[2], VAO[2];
	glGenVertexArrays(2, &VAO[1]);
	glGenBuffers(2, &VBO[1]);

    // ================================
    // buffer setup Cubes
    // ===============================
    glBindVertexArray(VAO[0]); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Color attribute
	glEnableVertexAttribArray(1);
    glBindVertexArray(0);


	glGenVertexArrays(2, &VAO[2]);
	glGenBuffers(2, &VBO[2]);
	// ================================
	// buffer setup Square Based Pyramids
	// ===============================
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);	// Vertex attributes stay the same
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Color attribute
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//++++++++++Build and compile shader program+++++++++++++++++++++
	GLuint shaderProgram = initShader("vert.glsl","frag.glsl");

	//++++++++++++++++++++++++++++++++++++++++++++++
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		time = glfwGetTime();
		
		// Calculate deltatime of current frame
		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();

		/* Render here */
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Draw the cube
		// Use cooresponding shader when setting uniforms/drawing objects
		glUseProgram(shaderProgram);
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);									//Object Color
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

		// Create transformations
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		//model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::translate(model, glm::vec3(0.0f,-1.0f,-3.0f));
		
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Get their uniform location
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3*12);


		//Draw Floor - This is where we draw the floor for the level


		// Draw the third cube
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 floor;

		floor = glm::scale(floor, glm::vec3(5.0f, 1.0f, 150.0f));
		//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		floor = glm::translate(floor, glm::vec3(0.0f, -2.0f, -0.5f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);									//Object Color
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floor));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);


		//Draw Destroyable Objects



		GLfloat destroyableDistance[100];
		glm::mat4 destroyableModels[100];

		int stateTwo = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		if ( time >= timeSinceDesRefresh + desRefreshTime)
		{
			timeSinceDesRefresh = time;
			destroyableObjects = destroyableObjects + 1;
			destroyableDistance[destroyableObjects] = time * 3.0f;		//Store Distance traveled for each object instance created

			int random = rand() % 5 + (-2);			//Random number between range 2 and -2
			destroyableObjectPos[destroyableObjects] = glm::vec3(random, 0.0f, destroyableDistance[destroyableObjects]);

			//Debug
			std::cout << "Destroyable " << destroyableObjects << " Object Instantiated" << std::endl;
			std::cout << destroyableObjectPos[destroyableObjects].x << std::endl;
			//std::cout << objectsInstantiated << std::endl;

		}

		glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);									//Object Color
		InstantiatedDestroyableObject(destroyableModels, view, projection, modelLoc, viewLoc, projLoc, VAO[0], destroyableDistance);



		// Draw the second cube
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 model2;

		//model2 = glm::rotate(model2, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model2 = glm::translate(model2, glm::vec3(-2.0f, -1.0f, -3.0f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);									//Object Color
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		// Draw the third cube
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 model3;

		model3 = glm::scale(model3, glm::vec3(1.0f, 1.0f, 1.0f));
		//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model3 = glm::translate(model3, glm::vec3(2.0f, -1.0f, -3.0f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);




		// Draw the pyramid
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 pyramidModel;

		pyramidModel = glm::scale(pyramidModel, glm::vec3(2.0f, 2.0f, 2.0f));
		//pyramidModel = glm::translate(pyramidModel, glm::vec3(glm::abs(glm::sin(time) * 4.5f), 2.0f, -4.0f));
		pyramidModel = glm::translate(pyramidModel, glm::vec3(-4.0f,-2.0f,-4.0f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);									//Object Color
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pyramidModel));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		// Draw the second pyramid
		// use shader
		glUseProgram(shaderProgram);

		// Create transformations
		glm::mat4 pyramidModel2;

		pyramidModel2 = glm::scale(pyramidModel2, glm::vec3(2.0f, 2.0f, 2.0f));
		//pyramidModel = glm::translate(pyramidModel, glm::vec3(glm::abs(glm::sin(time) * 4.5f), 2.0f, -4.0f));
		pyramidModel2 = glm::translate(pyramidModel2, glm::vec3(-1.0f, -2.0f, -4.0f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);									//Object Color
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pyramidModel2));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);



		//Instantiated Objects

		// Draw the first instantiated cube
		// use shader
		glUseProgram(shaderProgram);


		// Create transformations
		glm::mat4 models[100];
		GLfloat distanceTraveled[100];

		model3 = glm::scale(model3, glm::vec3(1.0f, 1.0f, 1.0f));
		//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model3 = glm::translate(model3, glm::vec3(2.0f, -1.0f, -3.0f));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);



		//On Click Method
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS && time >= timeLastPressed + pressDelayTime)
		{
			timeLastPressed = time;
			objectsInstantiated = objectsInstantiated + 1;
			distanceTraveled[objectsInstantiated] = time;		//Store Distance traveled for each object instance created

			//Debug
			std::cout << "Key Pressed" << std::endl;
			std::cout << objectsInstantiated << std::endl;
			
		}

		InstantiatedBullet(models, view, projection, modelLoc, viewLoc, projLoc, VAO[0], distanceTraveled);

		models[objectsInstantiated] = glm::scale(models[objectsInstantiated], glm::vec3(1.0f, 1.0f, 1.0f));
		//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//models[objectsInstantiated] = glm::translate(models[objectsInstantiated], glm::vec3(0.0f, -1.0f, -time + timeLastPressed));

		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Pass them to the shaders
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[objectsInstantiated]));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw object
		//glBindVertexArray(VAO[0]);
		//glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
		
		/*
		for (glm::mat4 modd : models)
		{

			}
		}*/


		/*
		for (int i = 0; i <= objectsInstantiated; i++)		//For every object instantiated, dray arrays
		{
			if (objectsInstantiated >= i) {		//For each object
				models[i] = glm::scale(models[i], glm::vec3(1.0f, 1.0f, 1.0f));
				//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				models[i] = glm::translate(models[i], glm::vec3(0.0f, 0.0, -distanceTraveled[i]));			//show object at position instantiated

				view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
				projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

				// Pass them to the shaders
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

				// draw object
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
			}
		}*/

		for (int i = 0; i <= objectsInstantiated; i++)		//For every object instantiated, dray arrays
		{
			if (objectsInstantiated >= i) {		//For each object
				models[i] = glm::scale(models[i], glm::vec3(1.0f, 1.0f, 1.0f));
				//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				//models[i] = glm::translate(models[i], glm::vec3(cameraPos.x -2.5f,cameraPos.y -2.5f, -distanceTraveled[i]));			//show object at position instantiated

				view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
				projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

				// Pass them to the shaders
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

				// draw object
				//glBindVertexArray(VAO[0]);
				//glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
			}
		}
		//InstantiatedObjects(models,view,projection, modelLoc, viewLoc, projLoc, VAO[0], distanceTraveled);

		/*
		if (time >= timeSinceDesRefresh + desRefreshTime) {
			timeSinceDesRefresh = time;
			for (int i = 0; i <= destroyableObjects; i++)
			{

				int random = rand() % 7 + (-3);				//Random number between 3 and -3
				//std::cout << random  << std::endl;				//Debug Random number

				desZPos = desZPos + 0.25f;
				//std::cout << desZPos << std::endl;				//Debug Destroyable Object Z axis number
				destroyableObjectPos[i] = glm::vec3(0.0f, 0.0f, desZPos);
				std::cout << destroyableObjectPos[i].z << std::endl;
			}
		}*/

		/*
		destroyableObjectPos[0] = glm::vec3(0.0f, 0.0f, 0.01f);
		destroyableObjectPos[1] = glm::vec3(0.0f, 0.01f, 1.0f);
		destroyableObjectPos[2] = glm::vec3(0.01f, 0.0f, 1.5f);

		for (int i = 0; i <= destroyableObjects; i++)		//For every object instantiated, dray arrays
		{
			if (destroyableObjects >= i) {		//For each object
				
				destroyableObjectPos[0] = glm::vec3(0.01f, 0, 0);
				destroyableModels[i] = glm::scale(destroyableModels[i], glm::vec3(1.0f, 1.0f, 1.0f));
				//model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				destroyableModels[i] = glm::translate(destroyableModels[i], destroyableObjectPos[i]);			//show object at position instantiated

				view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
				projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

				// Pass them to the shaders
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(destroyableModels[i]));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

				// draw object
				glBindVertexArray(VAO[0]);
				glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
				//std::cout << destroyableObjectPos[i].z << std::endl;

			}
		}*/

		

	
		glBindVertexArray(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO[2]);
	glDeleteBuffers(1, &VBO[2]);

	glfwTerminate();
	return 0;
}

void on_click(int key) {

	//Debug 
	if (key == GLFW_KEY_P) {
		std::cout << "Key Pressed" << std::endl;
	}
}

 //Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	on_click(key);
}



void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat) xpos;
		lastY = (GLfloat) ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat) xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat) ypos; // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat) xpos;
	lastY = (GLfloat) ypos;

	GLfloat sensitivity = (GLfloat) 0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}