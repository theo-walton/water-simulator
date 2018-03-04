
#version 410 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 perspective;

out vec3 color_out;

void	main()
{
	gl_Position = perspective * vec4(vertex.xy, vertex.z + 0, 1);
	float dot = max( dot( vec3(perspective * vec4(normal, 0)), vec3(0, 0, -1)), 0.1);
	color_out = color * dot;
}