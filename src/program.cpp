// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"

#include "gloom/shader.hpp"

using namespace Gloom;



void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.3f, 0.5f, 0.8f, 1.0f);


    // Set up your scene here (create Vertex Array Objects, etc.)





	static const GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	static const GLint indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};


	/*
	Next, ensure that your Shader is loaded and bound before you construct your VAO(s). Load and compile each of the shaders we would like to use
	You can now use the glGetAttribLocation() function to obtain the indexes which
	OpenGL has assigned to your inputs:
	*/
	Shader shader;	shader.attach("shaders/simple.vert");	shader.attach("shaders/simple.frag");
	GLuint mProgram = shader.get();
	shader.link();

	/*
	You should set the indexes passed in to glVertexAttribPointer() and
	glEnableVertexAttribArray() while setting up the VAO.
	*/
	int attributeLocation = glGetAttribLocation(mProgram, "position");

	/* Uniform are constants across all shaders, and cannot be changed by shader, must be changed by GPU*/
	glUniform4f(attributeLocation, 1.0f, 2.0f, 3.0f, 4.0f);


	// Create and bind VAO, Vertex Array Object. VAO are objects holding data on how to draw a specific object. A VAO contains 0-16 VBO's
	// VBO, Vertex Buffer Object, is some ombject holding data, it may be vertices, colors...
	// EBO,ELement Buffer Object. Defines how the coordinates of the vertices in the VBO are to be combined into primitives. It is another VBO that the VAO holds
	GLuint VAO = 0, VBO = 0, EBO = 0; // Create ID's to reference the given VAO and VBO
	glGenVertexArrays(1, &VAO); // generate 1 VAO from array of VAO id's. We use only 1 here
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO); // Sets this VAO as currently active VAO, only 1 can be active at a time. Drawing mutiple types of object can be done by switching out active bound VAO. Usually we bind, draw with, and unbind VAO's

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer and.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Call to glBufferData with given data (vertices) now puts the data into bound VBO


	glVertexAttribPointer(
		attributeLocation, // Vertex attribute pointer index in VAO
		3, // Number of components per vertex attribute {1,2,3,4}
		GL_FLOAT, // Type, datatype for each component in the array
		GL_FALSE, // Normalized, specified if data in buffer should be normalized (-1 to 1 for signed and 0.1 for unsigned) 
		0, // Stride, specifies byte offset between consecutive vertex attributes in array. Array contain 3 vertex + 2 texture: x,y,z,u,w,x,y,z... then the stride between x is 3*4 byte + 2*4 byte = 20 byte. 0 Lets OpenGL deduce
		0 // # Offset in bytes to forst component of first vertex attribute in array
	);
	// Enable Vertex Attribute Array to serve as input to rendering pipeline. glVertexAttribPointer and glEnableVertexAttribArray() ensures VA linked to shader inputs when issuing draw call
	glEnableVertexAttribArray(attributeLocation); // Enable this vertex attribute pointer for the VAO



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // EBO has special type of buffer GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices) / sizeof(indices[0]), indices, GL_STATIC_DRAW);


    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw your scene here

		shader.activate();
		glDrawElements( //glDrawElements will cause a draw call to be issued, and use the Vertex Attributes specified in the VAO as input to the rendering pipeline.
			GL_TRIANGLES,		// Mode parameter specifies the type of primitive you’d like to draw, TRIANGLE, LINE, POINTS
			6,					// Count parameter specifies how many elements from the index buffer should be drawn.
			GL_UNSIGNED_INT,	// Type specifies the data type of the values in your index buffer
			0					// Indices specifies the start index in your index buffer to start drawing from
		);
		shader.deactivate();


        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window);

        // Flip buffers
        glfwSwapBuffers(window);
    }



	//  de-allocate all resources once they've outlived their purpose:
	shader.destroy();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


}


void handleKeyboardInput(GLFWwindow* window)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}





