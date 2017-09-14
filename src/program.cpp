#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include <math.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


using namespace Gloom;



glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 camDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


bool firstMouse = true;
float yaw = 0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;



// DATA POINTS, INDICES AND COLORS
static  GLfloat vertices[] = {
	0.1f,  0.1f, 0.0f,  // top right
	0.1f, -0.1f, 0.0f,  // bottom right
	-0.1f, -0.1f, 0.0f,  // bottom left

	0.4f,  0.1f, 0.0f,  // top right
	0.4f, -0.1f, 0.0f,  // bottom right
	0.2f, -0.1f, 0.0f,  // bottom left

	0.7f,  0.1f, 0.0f,  // top right
	0.7f, -0.1f, 0.0f,  // bottom right
	0.5f, -0.1f, 0.0f,  // bottom left

	0.1f,  0.4f, 0.0f,  // top right
	0.1f, 0.2f, 0.0f,  // bottom right
	-0.1f, 0.2f, 0.0f,  // bottom left

	0.1f,  0.7f, 0.0f,  // top right
	0.1f, 0.5f, 0.0f,  // bottom right
	-0.1f, 0.5f, 0.0f,  // bottom left

};
static int verticesLength = 45;

static  GLint indices[] = {  // note that we start from 0!
	2, 1 ,0,   // first triangle
	5, 4, 3,    // second triangle
	8, 7, 6,
	11, 10, 9,
	14, 13, 12

};
static int indicesLength = 15;


static GLfloat colors[] = {
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.4f, 0.1f, 0.1f, 1.0f,

	0.1f, 0.1f, 0.3f, 1.0f,
	0.1f, 0.0f, 0.4f, 1.0f,
	0.6f, 1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.8f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 0.6f, 0.0f, 1.0f,
	1.0f, 0.3f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.2f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.3f, 1.0f,
	0.7f, 1.0f, 1.0f, 1.0f

};
static int colorsLength = 60;

static GLfloat vertices2[] = {
	0.6f, -0.8f, -1.2f,
	0.0f, 0.4f, 0.0f,
	-0.8f, -0.2f, 1.2f
};
static int vertices2Length = 9;

static GLint indices2[]{
	0, 1, 2
};
static int indices2Length = 2;

static GLfloat colors2[]{
	1.0f, 1.0f, 0.0f, 0.7f,
	1.0f, 0.0f, 0.0f, 0.1f,
	0.0f, 1.0f, 1.0f, 1.0f
};
static int colors2Length = 12;


static GLfloat vertices3[] = {
	0.0f, -0.8f, 0.0f,
	0.8f, 0.0f, 0.0f,
	-0.8f, 0.0f, 0.0f,

	-0.4f, -0.8f, 0.2f,
	0.6f, -0.6f, 0.2f,
	0.2f, -0.2f, 0.2f,

	0.4f, -0.8f, 0.4f,
	0.4f, 0.8f, 0.4f,
	-0.8f, 0.6f, 0.4f
};
static GLfloat vertices3ExhangedDepths[] = {
	0.0f, -0.8f, 0.5f,
	0.8f, 0.0f, 0.5f,
	-0.8f, 0.0f, 0.5f,

	-0.4f, -0.8f, 0.9f,
	0.6f, -0.6f, 0.9f,
	0.2f, -0.2f, 0.9f,

	0.4f, -0.8f, 0.0f,
	0.4f, 0.8f, 0.0f,
	-0.8f, 0.6f, 0.0f
};
static int vertices3Length = 27;


static GLint indices3[]{
	0, 1, 2,
	3, 4, 5,
	6, 7, 8
};
static GLint indices3BackToFront[]{
	6, 7, 8,
	3, 4, 5,
	0, 1, 2
};
static int indices3Length = 9;


static GLfloat colors3[]{
	1.0f, 0.0f, 0.0f, 0.3f,
	1.0f, 0.0f, 0.0f, 0.3f,
	1.0f, 0.0f, 0.0f, 0.3f,

	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.6f,

	0.0f, 0.0f, 1.0f, 0.9f,
	0.0f, 0.0f, 1.0f, 0.9f,
	0.0f, 0.0f, 1.0f, 0.9f
};
static GLfloat colors3Exchanged[]{
	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.6f,

	0.0f, 0.0f, 1.0f, 0.9f,
	0.0f, 0.0f, 1.0f, 0.9f,
	0.0f, 0.0f, 1.0f, 0.9f,

	1.0f, 0.0f, 0.0f, 0.3f,
	1.0f, 0.0f, 0.0f, 0.3f,
	1.0f, 0.0f, 0.0f, 0.3f,
};
static int colors3Length = 36;


void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Enabling OpenGL transparacy given by the alpha values on the colors
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set default colour after clearing the colour buffer
	glClearColor(0.3f, 0.5f, 0.8f, 1.0f);

	// Hides cursor when window is active
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Callback for when mouse position changes
	glfwSetCursorPosCallback(window, mouse_callback); 
	//Creating and linking of shaders are simplified with the handout code for the shader
	Shader shader;	shader.attach("../gloom/shaders/simple.vert");	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

	// setting up VAO and VBO's with given data. Returns ID to created VAO
	int VAO = setupVOA(vertices, verticesLength, colors, colorsLength, indices, indicesLength);

	// Test transformation matrices Task 4A
	glm::mat4 transformationMatrix2 = glm::mat4(1.0);
	glm::mat4 transformationMatrix3 = glm::translate(glm::vec3(0.3, 0.3, 0));
	glm::mat4 transformationMatrix4 = glm::rotate(30.0f, glm::vec3(0, 0, 1));
	glm::mat4 transformationMatrix5 = transformationMatrix3 * transformationMatrix4;

	// Test code for rotating about center
	/*
	glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camUp = glm::cross(camDirection, camRight);
	float radius = 0.6f;  // why can I not view objects from greater distances????????????????????
	float camX;
	float camZ;
	camX = sin(glfwGetTime()) * radius;
	camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0, camZ), camTarget, camUp);
	*/

	// Projection matrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)16 / (float)9, 1.0f, 100.0f);  // TASK 4 b)

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draw your scene here
		glEnableVertexAttribArray(VAO); // Enable this vertex attribute pointer for the VAO

		// SHADER TIME
		shader.activate();

		/*
		PRO PERFORMANCE TIP: Its best to do matrix multiplication on cpu, not on gpu if the matrix used can be passed as a uniform.
		Rather do 5 matrix multiplication on cpu with cpu clock speed than #cores * 5 multiplication on gpu with GPU core clock speed
		*/

		// Create transformatin matrices
		glm::mat4 I = glm::mat4(1.0f);

		glm::mat4 translationMatrix = glm::translate(-camPosition); // must translate world in negative direction to create illusion of moving in positive
		glm::mat4 viewBasisMatrix = transpose(glm::mat4(glm::vec4(camRight, 0.0f), glm::vec4(camUp, 0.0f), glm::vec4(-camDirection, 0.0f), glm::vec4(0, 0, 0, 1)));
		glm::mat4 viewTransform = viewBasisMatrix * translationMatrix;

		glm::mat4 rotYaw = glm::rotate(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotPitch = glm::rotate(glm::radians(pitch), camRight);


		// Create final transformation matrix by multiplying all the transformation in the desired order
		//glm::mat4 transformationMatrix = projectionMatrix * rotationPitch* rotationYaw * translationMatrix * I;
		glm::mat4 transformationMatrixWCS2ECSVersion = projectionMatrix * viewTransform;

		glm::mat4 transformationMatrixRotationVersion = projectionMatrix * rotYaw * rotPitch * translationMatrix;

		// This creates the same translateion + rotation matrix as above. This is how you are supposed to do int
		// glm::mat4 view = glm::lookAt(camPosition, camPosition + camFront, camUp); // param: position, target/inverse direction, up
		
		// Pass the transformation matrix to the shader
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(transformationMatrixWCS2ECSVersion));

		// Draw data from VBO's in VAO
		glDrawElements( //glDrawElements will cause a draw call to be issued, and use the Vertex Attributes specified in the VAO as input to the rendering pipeline.
			GL_TRIANGLES,		// Mode parameter specifies the type of primitive you�d like to draw, TRIANGLE, LINE, POINTS
			15,					// Count parameter specifies how many elements from the index buffer should be drawn.
			GL_UNSIGNED_INT,	// Type specifies the data type of the values in your index buffer
			0					// Indices specifies the start index in your index buffer to start drawing from
		);
		shader.deactivate();
		// SHADER END
		glEnableVertexAttribArray(0); // Unbind currently bound VAO
		// Handle currently pressed keyboard keys to update position and angles
		handleKeyboardInput(window);
		// Handle other events
		glfwPollEvents();
		// Flip buffers
		glfwSwapBuffers(window);
	}
	//  de-allocate all resources once they've outlived their purpose:
	shader.destroy();
}

/**
	Called for every drawn frame, handles all kayboard input. Translates and rotates
*/
void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// Calculates rate of change based on how long it was since last fram update
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float camSpeed = 2.5f * deltaTime;

	// Translation !!! Note that these controls will move left right forward backwards relative to the camera's orientation, and not the world, therefore it is kind of a floating object in space and not a standing one
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camPosition += camSpeed * camDirection;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camPosition -= camSpeed * camDirection;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camPosition -= glm::normalize(glm::cross(camDirection, camUp)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camPosition += glm::normalize(glm::cross(camDirection, camUp)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camPosition += up * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camPosition -= up *camSpeed;

	// Rotation
	float yoffset = 0;
	float xoffset = 0;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		yoffset += camSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		yoffset -= camSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		xoffset -= camSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		xoffset += camSpeed;

	// Need to update this for being able to switch between keyboard and mouse rotation
	lastX += xoffset;
	lastY += yoffset;

	float sensitivity = 15; // need higher sensitivity than with mouse for this to feel efficient
	updateAngles(xoffset, yoffset, sensitivity);
}


/**
	Called whenever a mouse movement is detected, calculates the offset from last position and updates angles

	@param window in focus, position of cursor in x and y direction
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse){ // First time detecting mouse movement we need to set this as initial position
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		/*
		printf("%f, %f\n", lastX, lastY);
		printf("camFront: %f, %f, %f\n\n", camFront.x, camFront.y, camFront.z);
		*/
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05; // nice sensitivity for mouse movement
	updateAngles(xoffset, yoffset, sensitivity); // updates angles and camFront
}

/**
	Updates angles based on offset in x in y direction, with a factor of sensitivity

	@param offset in x and y direction, desired sensitivity for change
*/
void updateAngles(float xoffset, float yoffset, float sensitivity) {
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Don't want to be able to spin vertically
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update the camera front vector with new angles and normalize it
	glm::vec3 front;
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	camDirection = glm::normalize(front);

	camRight = glm::normalize(glm::cross(camDirection, up));
	camUp = glm::normalize(glm::cross(camRight, camDirection));

	// Debug printing
	/*
	printf("Angles: %f, %f\n", yaw, pitch);
	printf("CamFront: %f, %f, %f\n", camFront.x, camFront.y, camFront.z);
	printf("CamPos: %f, %f, %f\n\n", camPosition.x, camPosition.y, camPosition.z);
	*/
}




int setupVOA(float* coordinates, int numCoordinates, float* colorValues, int numColorValues, int* indices, int numIndices) {

	// Create and bind VAO, Vertex Array Object. VAO are objects holding data on how to draw a specific object. A VAO contains 0-16 VBO's
	// VBO, Vertex Buffer Object, is some ombject holding data, it may be vertices, colors...
	// EBO,ELement Buffer Object. Defines how the coordinates of the vertices in the VBO are to be combined into primitives. It is another VBO that the VAO holds
	GLuint VAO = 0, VBOIndices = 0, VBOColors = 0, EBO = 0; // Create ID's to reference the given VAO and VBO
	glGenVertexArrays(1, &VAO); // generate 1 VAO from array of VAO id's. We use only 1 here

	glBindVertexArray(VAO); // Sets this VAO as currently active VAO, only 1 can be active at a time. Drawing mutiple types of object can be done by switching out active bound VAO. Usually we bind, draw with, and unbind VAO's

	glGenBuffers(1, &VBOIndices);
	glBindBuffer(GL_ARRAY_BUFFER, VBOIndices); // Any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer and.
	glBufferData(GL_ARRAY_BUFFER, numCoordinates * sizeof(float), coordinates, GL_STATIC_DRAW); // Call to glBufferData with given data (vertices) now puts the data into bound VBO

	glVertexAttribPointer(
		0, // Vertex attribute pointer index in VAO
		3, // Number of components per vertex attribute {1,2,3,4}
		GL_FLOAT, // Type, datatype for each component in the array
		GL_FALSE, // Normalized, specified if data in buffer should be normalized (-1 to 1 for signed and 0.1 for unsigned)
		0, // Stride, specifies byte offset between consecutive vertex attributes in array. Array contain 3 vertex + 2 texture: x,y,z,u,w,x,y,z... then the stride between x is 3*4 byte + 2*4 byte = 20 byte. 0 Lets OpenGL deduce
		0 // # Offset in bytes to forst component of first vertex attribute in array
	);
	// Enable Vertex Attribute Array to serve as input to rendering pipeline. glVertexAttribPointer and glEnableVertexAttribArray() ensures VA linked to shader inputs when issuing draw call
	glEnableVertexAttribArray(0); // Enable this vertex attribute pointer for the VAO

	// Generate VBO for colors
	glGenBuffers(1, &VBOColors);
	glBindBuffer(GL_ARRAY_BUFFER, VBOColors);
	glBufferData(GL_ARRAY_BUFFER, numColorValues * sizeof(float), colorValues, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1); // ?????? is this correct? Can it be done any other way besides hardcoding?




	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // EBO has special type of buffer GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	return VAO;
}
