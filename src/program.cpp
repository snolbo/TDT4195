#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "gloom/sceneGraph.hpp"
#include <glm/ext.hpp>

#include <iostream>

#include <math.h>
#include <vector>


using namespace Gloom;

glm::vec3 camPosition = glm::vec3(0.0f, 1.0f, 8.0f);
glm::vec3 camDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float deltaAnimationTime;

// Variables used for scenenodes and updating them
SceneNode* root;
bool firstTarget = true;
std::vector<glm::vec3> destinationPoints;
glm::vec3 destinationLocation;
int pointIndex = 0;

// Variables used for camera movement
bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float fov = 45.0f;



// Data for body parts:
static GLfloat headVertices[] = {
	// head front
	-0.4f, 0.0f, 0.4f,
	 0.4f, 0.0f, 0.4f,
	 0.4f, 0.8f, 0.4f,
	-0.4f, 0.8f, 0.4f,

	// head back
	-0.4f, 0.0f, -0.4f,
	 0.4f, 0.0f, -0.4f,
	 0.4f, 0.8f, -0.4f,
	-0.4f, 0.8f, -0.4f
};

static GLfloat torsoVertices[] = {
	// torso front
	-0.4f, 0.0f, 0.2f,
	0.4f, 0.0f, 0.2f,
	0.4f, 1.2f, 0.2f,
	-0.4f, 1.2f, 0.2f,

	// torso back
	-0.4f, 0.0f, -0.2f,
	0.4f, 0.0f, -0.2f,
	0.4f, 1.2f, -0.2f,
	-0.4f, 1.2f, -0.2f
};

static GLfloat armVertices[] = {
	// arm front
	-0.2f, 0.0f, 0.2f,
	 0.2f, 0.0f, 0.2f,
	 0.2, 1.2f, 0.2f,
	-0.2f, 1.2f, 0.2f,

	// arm back
	-0.2f, 0.0f, -0.2f,
	 0.2f, 0.0f, -0.2f,
	 0.2f, 1.2f, -0.2f,
	 -0.2f, 1.2f, -0.2f
};


static GLfloat legVertices[] = {
	// leg front
	-0.2f, 0.0f, 0.2f,
	 0.2f, 0.0f, 0.2f,
	 0.2f, 1.2f, 0.2f,
	-0.2f, 1.2f, 0.2f, 	

	// leg back
	-0.2f, 0.0f, -0.2f,
	 0.2f, 0.0f, -0.2f,
	 0.2f, 1.2f, -0.2f,
	-0.2f, 1.2f, -0.2f,

};

int bodyPartVerticesLength = 24;

static GLint indicesBodyPart[] = {
	// front and back
	0, 1, 2,
	0, 2, 3,

	4, 7, 6,
	4, 6, 5,

	// left side
	4, 0, 3,
	4, 3, 7,

	// right side
	1, 5, 6,
	1, 6, 2,

	//top 
	3, 2, 6,
	3, 6, 7,

	// bot
	4, 5, 1,
	4, 1, 0
};
int indicesBodyPartLength = 36;

static GLfloat bodyPartColors[]{
	// front
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	
	//back
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f

};
int bodyPartColorLength = 32;

// Data for terrain
GLfloat terrainVert[420];
GLint terrainInd[210];
GLfloat terrainCol[560];
int terrainVertLen = 420;
int terrainIndLen = 210;
int terrainColLen = 560;

// Buffer arrays and ID placeholders
GLuint VAO[7], VBOIndices[7], VBOColors[7], EBO[7];
int leftLegVAO, rightLegVAO, torsoVAO, leftArmVAO, rightArmVAO, headVAO, terrainVAO;

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Hides cursor when window is active
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Callback for when mouse position changes
	glfwSetCursorPosCallback(window, mouse_callback);

	//Creating and linking of shaders are simplified with the handout code for the shader
	Shader shader;
	shader.attach("../gloom/shaders/simple.vert");
	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

	generateTerrainData();

	// Generate buffers for all VAO and their data
	glGenVertexArrays(7, VAO); // generate 1 VAO from array of VAO id's. We use only 1 here
	glGenBuffers(7, VBOIndices);
	glGenBuffers(7, VBOColors);
	glGenBuffers(7, EBO);


	// setting up VAO and VBO's with given data. Returns ID to created VAO
	leftLegVAO = setupVOA(&VAO[0], &VBOIndices[0], &VBOColors[0], &EBO[0],  legVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	rightLegVAO = setupVOA(&VAO[1], &VBOIndices[1], &VBOColors[1], &EBO[1], legVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	torsoVAO = setupVOA(&VAO[2], &VBOIndices[2], &VBOColors[2], &EBO[2], torsoVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	leftArmVAO = setupVOA(&VAO[3], &VBOIndices[3], &VBOColors[3], &EBO[3], armVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	rightArmVAO = setupVOA(&VAO[4], &VBOIndices[4], &VBOColors[4], &EBO[4], armVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	headVAO = setupVOA(&VAO[5], &VBOIndices[5], &VBOColors[5], &EBO[5], headVertices, bodyPartVerticesLength, bodyPartColors, bodyPartColorLength, indicesBodyPart, indicesBodyPartLength);
	terrainVAO = setupVOA(&VAO[6], &VBOIndices[6], &VBOColors[6], &EBO[6], terrainVert, terrainVertLen, terrainCol, terrainColLen, terrainInd, terrainIndLen);

	// get root node for scene graph
	root = constructRootSceneNode();

	// Projection matrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)16 / (float)9, 1.0f, 100.0f);  // TASK 4 b)

	deltaAnimationTime = getTimeDeltaSeconds();

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		deltaAnimationTime = getTimeDeltaSeconds();

		shader.activate();
		/*
		PRO PERFORMANCE TIP: Its best to do matrix multiplication on cpu, not on gpu if the matrix used can be passed as a uniform.
		Rather do 5 matrix multiplication on cpu with cpu clock speed than #cores * 5 multiplication on gpu with GPU core clock speed
		*/
		updateSceneGraph(root);

		// Create transformation matrix
		glm::mat4 translationMatrix = glm::translate(-camPosition); // must translate world in negative direction to create illusion of moving in positive
		glm::mat4 rotYaw = glm::rotate(glm::radians(yaw), up);
		glm::mat4 rotPitch = glm::rotate(glm::radians(pitch), camRight);
		glm::mat4 transformationMatrixRotationVersion = projectionMatrix * rotYaw * rotPitch * translationMatrix;

		// This creates the same translateion + rotation matrix as above. This is how you are supposed to do it
		// glm::mat4 view = glm::lookAt(camPosition, camPosition + camFront, camUp); // param: position, target/inverse direction, up

		/* // Using viewBasis instead:
		glm::mat4 viewBasisMatrix = transpose(glm::mat4(glm::vec4(camRight, 0.0f), glm::vec4(camUp, 0.0f), glm::vec4(-camDirection, 0.0f), glm::vec4(0, 0, 0, 1)));
		glm::mat4 viewTransform = viewBasisMatrix * translationMatrix;
		glm::mat4 transformationMatrixWCS2ECSVersion = projectionMatrix * viewTransform;
		*/

		drawSceneNodes(root, transformationMatrixRotationVersion);


		shader.deactivate();

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

/*
	Generates vertices, colors and indices for drawing the terrain
*/
void generateTerrainData() {
	// Size of the terrain
	int rows = 6;
	int cols = 8;

	// Generate vertex data for terrain
	for (int i = 0; i < (rows - 1); i++) {
		for (int j = 0; j < (cols - 1); j++) {

			int index = 12 * i*(cols - 1) + 12 * j;

			terrainVert[index + 0] = (j + 0);
			terrainVert[index + 1] = 0.0f;
			terrainVert[index + 2] = -(i + 0);

			terrainVert[index + 3] = (j + 1);
			terrainVert[index + 4] = 0.0f;
			terrainVert[index + 5] = -(i + 0);

			terrainVert[index + 6] = (j + 1);
			terrainVert[index + 7] = 0.0f;
			terrainVert[index + 8] = -(i + 1);

			terrainVert[index + 9] = (j + 0);
			terrainVert[index + 10] = 0.0f;
			terrainVert[index + 11] = -(i + 1);
			// printf(" (%f %f %f)   (%f %f %f)   (%f %f %f)   (%f %f %f) ", terrainVert[index], terrainVert[index+1], terrainVert[index+2], terrainVert[index+3], terrainVert[index+4], terrainVert[index+5], terrainVert[index+6], terrainVert[index+7], terrainVert[index+8], terrainVert[index+9], terrainVert[index+10], terrainVert[index+11]);
			// printf(" (%d %d %d) (%d %d %d) (%d %d %d) (%d %d %d)       ", (index + 0), (index + 1), (index + 2), (index + 2), (index + 3), (index + 4), (index + 5), (index + 6), (index + 7), (index + 8), (index + 9), (index + 11));
		}
		//printf("\n");
	}
	//printf("\n");

	// Generate index data for terrainVertices
	for (int ii = 0; ii < (rows - 1)*(cols - 1); ii++) {
		int index = ii * 6;
		int indexV = 4 * ii;
		terrainInd[index + 0] = indexV + 0;
		terrainInd[index + 1] = indexV + 1;
		terrainInd[index + 2] = indexV + 2;

		terrainInd[index + 3] = indexV + 0;
		terrainInd[index + 4] = indexV + 2;
		terrainInd[index + 5] = indexV + 3;

		//printf("  (%d %d %d) (%d %d %d)  ", indexV, indexV + 1, indexV + 2, indexV, indexV + 2, indexV + 3);
	}


	// Generate colordata for terrainVertices
	int r = 1.0f;
	int g = 0.0f;
	int b = 0.0f;
	int color1 = 1;
	for (int ii = 0; ii < (rows - 1)*(cols - 1) * 4; ii++) {
		if (ii % 4 == 0) {
			color1 = !color1;
			if (color1) {
				r = 1.0f;
				g = 1.0f;
				b = 0.0f;
				//r = 1.0f;
				//g = 0.0f;
				//b = 0.0f;
			}
			else {
				r = 0.0f;
				g = 0.0f;
				b = 1.0f;
				//r = 0.0f;
				//g = 0.0f;
				//b = 1.0f;
			}
		}
		int index = 4 * ii;
		terrainCol[index + 0] = r;
		terrainCol[index + 1] = g;
		terrainCol[index + 2] = b;
		terrainCol[index + 3] = 1.0f;
	}
}


/*
	Creates scenenones, sets initial values, and relate them to eachother. Also chooses which point to visit first
*/
SceneNode* constructRootSceneNode() {

	// Locations to visit
	glm::vec3 point1 = glm::vec3(1.5, 0.24, -0.5);
	glm::vec3 point2 = glm::vec3(4.5, 0.24, -1.5);
	glm::vec3 point3 = glm::vec3(2.5, 0.24, -3.5);
	glm::vec3 point4 = glm::vec3(4.5, 0.24, -4.5);
	glm::vec3 point5 = glm::vec3(5.5, 0.24, -2.5);


	destinationPoints.push_back(point1);
	destinationPoints.push_back(point2);
	destinationPoints.push_back(point3);
	destinationPoints.push_back(point4);
	destinationPoints.push_back(point5);

	destinationLocation = destinationPoints.at(pointIndex);

	SceneNode* root = createSceneNode();

	SceneNode* leftArm = createSceneNode();
	SceneNode* rightArm = createSceneNode();
	SceneNode* leftLeg = createSceneNode();
	SceneNode* rightLeg = createSceneNode();
	SceneNode* head = createSceneNode();
	SceneNode* torso = createSceneNode();
	SceneNode* terrain = createSceneNode();

	addChild(root, torso);
	addChild(root, terrain);

	addChild(torso, head);

	addChild(torso, leftArm);
	addChild(torso, rightArm);

	addChild(torso, leftLeg);
	addChild(torso, rightLeg);

	terrain->vertexArrayObjectID = terrainVAO;
	torso->vertexArrayObjectID = torsoVAO;
	head->vertexArrayObjectID = headVAO;

	leftLeg->vertexArrayObjectID = leftLegVAO;
	rightLeg->vertexArrayObjectID = rightLegVAO;

	leftArm->vertexArrayObjectID = leftArmVAO;
	rightArm->vertexArrayObjectID = rightArmVAO;

	root->x = 0.0f;
	root->y = 0.0f;
	root->z = 0.0f;

	terrain->x = 0.0f;
	terrain->y = 0.0f;
	terrain->z = 0.0f;
	
	torso->x = 0.0f;
	torso->y = 0.24;
	torso->z = 0.0f;
	torso->rotationY = 0;
	torso->directionVector = glm::vec3(0.0f, 0.0f, 1.0f);
	torso->behavior = 2;
	torso->rotationDirection = up;

	head->y = 1.2f;

	leftArm->x = -0.6;
	leftArm->yref = 1.2;
	leftArm->behavior = 1;
	leftArm->rotationAngularDirection = -1;

	rightArm->x = 0.6;
	rightArm->yref = 1.2;
	rightArm->behavior = 1;

	leftLeg->y = -1.2f;
	leftLeg->x = -0.2;
	leftLeg->yref = 1.2;

	rightLeg->y = -1.2f;
	rightLeg->x = 0.2;
	rightLeg->yref = 1.2;
	rightLeg->rotationAngularDirection = -1;

	leftLeg->behavior = 1;
	rightLeg->behavior = 1;
		
	root->currentTransformationMatrix = glm::mat4(1.0f);

	return root;
}

/*
	Updates graph node to handle animation
*/
void updateSceneGraph(SceneNode* node) {

	glm::mat4 nodeTransformationMatrix = glm::mat4(1.0f);
	glm::vec3 relativePosition = glm::vec3(node->x, node->y, node->z);

	// Updates translation and general orientation of the body. 
	if (node->vertexArrayObjectID == torsoVAO) {

		glm::vec3 distance = destinationLocation - relativePosition;
		// If target reached or first time selecting target, choose new target from list, update moving direction and orientation
		if (glm::length(distance) < 0.2 || firstTarget) {
			pointIndex++;
			if (pointIndex > destinationPoints.size()-1) {
				pointIndex = 0;
			}
			destinationLocation = destinationPoints.at(pointIndex);
			float angle = glm::dot(glm::normalize(node->directionVector), glm::normalize(destinationLocation - relativePosition));
			if (abs(angle) >0.999) {
				angle = angle - 0.0001;
			}
			//printf("angle: %f  dir> %f %f %f   dest %f %f %f\n", (angle), node->directionVector.x, node->directionVector.y, node->directionVector.z, destinationLocation.x, destinationLocation.y, destinationLocation.z);
			glm::vec3 rotationCross = glm::cross(node->directionVector, destinationLocation - relativePosition);
			int dir = 0;
			// find out if we need to rotate to left or right
			if (rotationCross.y < 0) {
				dir = -1;
			}
			else {
				dir = 1;
			}
			angle = glm::acos(angle);
			angle = dir*angle;
			node->rotationY += angle;
			firstTarget = false;
			node->directionVector = glm::normalize(destinationLocation - relativePosition);
		}
		

		glm::mat4 scale = glm::scale(glm::vec3(0.2f, 0.2f, 0.2f));
		glm::mat4 rotY = glm::rotate(node->rotationY, up);
		nodeTransformationMatrix =  rotY * scale * nodeTransformationMatrix;
	

		node->x += node->directionVector.x * deltaAnimationTime/2 ;
		node->y += node->directionVector.y * deltaAnimationTime/2 ;
		node->z += node->directionVector.z * deltaAnimationTime/2 ;
	}
	else if (node->behavior == 1) { // arms and legs that rotate
		int maxDegrees = 60;
		if (glm::degrees(node->rotationX) > maxDegrees) {
			node->rotationAngularDirection = -1;
		}
		else if (glm::degrees(node->rotationX) < -maxDegrees) {
			node->rotationAngularDirection = 1;
		}
		node->rotationX += node->rotationAngularDirection * deltaAnimationTime * 1.5;
		glm::vec3 referencePoint = glm::vec3(node->xref, node->yref, node->zref);
		glm::mat4 referencePointsTranslation = glm::translate(-referencePoint);
		glm::mat4 invReferencePointsTranslation = glm::translate(referencePoint);
		glm::mat4 rotX = glm::rotate(node->rotationX, node->rotationDirection);

		nodeTransformationMatrix = invReferencePointsTranslation * rotX * referencePointsTranslation;
	}


	glm::mat4 translationMatrix = glm::translate(relativePosition);
	nodeTransformationMatrix = translationMatrix * nodeTransformationMatrix;
	node->currentTransformationMatrix = nodeTransformationMatrix;

	// Recursive call on all children
	for (auto child : node->children) {
		if (child->vertexArrayObjectID == 0) {
			continue;
		}
		updateSceneGraph(child);
	}
}

/*
	Draws scenenodes all calls this function in a recursive matter on all of its children
*/
void drawSceneNodes(SceneNode* node, glm::mat4 accumulatedTransformationMatrix) {
	int indexLength = indicesBodyPartLength;
	if (node->vertexArrayObjectID == terrainVAO) { // LOLOLOLOLOLOLOLOL HOW TO HANDLE THIS????
		indexLength = terrainIndLen;
	}

	if (node->vertexArrayObjectID != -1) {
		accumulatedTransformationMatrix *= node->currentTransformationMatrix;
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(accumulatedTransformationMatrix));
		glBindVertexArray(node->vertexArrayObjectID);
		glEnableVertexAttribArray(node->vertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, 0);
	}

	// Calls this function for all this node's children
	for (auto child : node->children) {
		drawSceneNodes(child, accumulatedTransformationMatrix);
	}
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
	float camSpeed = 5.0f * deltaTime;

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
	pitch -= yoffset;

	// Don't want to be able to spin vertically
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update the camera front vector with new angles and normalize it
	glm::vec3 front;
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = -sin(glm::radians(pitch));
	front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	camDirection = glm::normalize(front);

	camRight = glm::normalize(glm::cross(camDirection, up));
	camUp = glm::normalize(glm::cross(camRight, camDirection));

	/*Debug printing
	printf("Angles: %f, %f\n", yaw, pitch);
	printf("CamFront: %f, %f, %f\n", camFront.x, camFront.y, camFront.z);
	printf("CamPos: %f, %f, %f\n\n", camPosition.x, camPosition.y, camPosition.z);
	*/
}



/*
	Sets up a VAO with the given buffers and returns it' ID
*/
int setupVOA(GLuint* VAO, GLuint* VBOIndices, GLuint* VBOColors, GLuint* EBO, float* coordinates, int numCoordinates, float* colorValues, int numColorValues, int* indices, int numIndices) {

	glBindVertexArray(*VAO); // Sets this VAO as currently active VAO, only 1 can be active at a time. Drawing mutiple types of object can be done by switching out active bound VAO. Usually we bind, draw with, and unbind VAO's
	
	// Generate VBO for vertices
	glBindBuffer(GL_ARRAY_BUFFER, *VBOIndices); // Any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer and.
	glBufferData(GL_ARRAY_BUFFER, numCoordinates * sizeof(float), coordinates, GL_STATIC_DRAW); // Call to glBufferData with given data (vertices) now puts the data into bound VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0); // Enable this vertex attribute pointer for the VAO

	// Generate VBO for colors
	glBindBuffer(GL_ARRAY_BUFFER, *VBOColors);
	glBufferData(GL_ARRAY_BUFFER, numColorValues * sizeof(float), colorValues, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Set indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO); // EBO has special type of buffer GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	return *VAO;
}
