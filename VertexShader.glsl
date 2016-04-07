#version 330

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(1.0);
}