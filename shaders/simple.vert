#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec4 color;

out layout(location = 0) vec4 colorV;



uniform layout(location = 0) mat4 transformationMatrix;


mat4 task3Matrix = mat4(	1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1);


void main()
{
	
    gl_Position =  transformationMatrix * vec4(position, 1.0f);
	colorV = color;
}
