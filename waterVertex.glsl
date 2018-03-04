
#version 410 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in float height;

uniform mat4 perspective;

out float height_out;
out float dot_out;

void	main()
{
	gl_Position = perspective * vec4(vertex, 1);
	dot_out = max( dot( vec3(perspective * vec4(normal, 0)), vec3(0, 0, -1)), 0);
	height_out = height;
}