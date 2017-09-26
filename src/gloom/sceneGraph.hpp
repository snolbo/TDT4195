#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <stack>
#include <vector>
#include <cstdio>
#include <stdbool.h>
#include <cstdlib> 
#include <ctime> 
#include <chrono>
#include <fstream>

#ifndef PI 
	#define PI 3.14159265
#endif

// Matrix stack related functions
std::stack<glm::mat4>* createEmptyMatrixStack();
void pushMatrix(std::stack<glm::mat4>* stack, glm::mat4 matrix);
void popMatrix(std::stack<glm::mat4>* stack);
glm::mat4 peekMatrix(std::stack<glm::mat4>* stack);

void printMatrix(glm::mat4 matrix);

// SceneGraph related functions

// In case you haven't got much experience with C or C++, let me explain this "typedef" you see below.
// The point of a typedef is that you it, as its name implies, allows you to define arbitrary data types based upon existing ones. For instance, "typedef float typeWhichMightBeAFloat;" allows you to define a variable such as this one: "typeWhichMightBeAFloat variableName = 5.0;". The C/C++ compiler translates this type into a float. 
// What is the point of using it here? A smrt person, while designing the C language, thought it would be a good idea for various reasons to force you to explicitly state that you are using a data structure datatype (struct). So, when defining a variable, you'd have to type "struct SceneNode node = ..." in the case of a SceneNode. Which can get in the way of readability.
// If we just use typedef to define a new type called "SceneNode", which really is the type "struct SceneNode", we can omit the "struct" part when creating an instance of SceneNode. 
typedef struct SceneNode {
	// A list of all children that belong to this node. 
	// For instance, in case of the scene graph of a human body shown in the assignment text, the "Upper Torso" node would contain the "Left Arm", "Right Arm", "Head" and "Lower Torso" nodes in its list of children.
	std::vector<SceneNode*> children;
	
	// The node's rotation relative to its parent
	float rotationX, rotationY, rotationZ;
	
	// The node's position relative to its parent
	float x, y, z;
	
	// The node's size
	float scaleFactor;
	
	// The node's speed at which it rotates (orbits)
	float rotationSpeedRadians;
	
	// A normalised vector defining the axis around which the node rotates
	glm::vec3 rotationDirection;
	
	// A transformation matrix representing the transformation of the node's location relative to its parent. This matrix is updated every frame.
	glm::mat4 currentTransformationMatrix;

	// The ID of the VAO containing the "appearance" of this SceneNode.
	int vertexArrayObjectID;
} SceneNode;

// Struct for keeping track of 2D coordinates
typedef struct int2 {
	int x, y;
} int2;

SceneNode* createSceneNode();
void addChild(SceneNode* parent, SceneNode* child);
void printNode(SceneNode* node);

// Utility functions
float random();
double getTimeDeltaSeconds();
float toRadians(float angleDegrees);
std::vector<int2> readCoordinatesFile(std::string filePath);

// For more details, see SceneGraph.cpp.