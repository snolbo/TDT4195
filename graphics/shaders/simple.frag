#version 430 core


in layout(location = 0) vec4 colorV;

out vec4 outputFrag;



void main()
{
    outputFrag = colorV;
}
