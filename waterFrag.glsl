
#version 410 core

in float height_out;
in float dot_out;

out vec4 color;

void	main()
{
	float ood = 1 - dot_out / 1.5;
	float transparency = min(height_out, 0.8);
	color = vec4(ood / 2, ood / 1.5, ood, transparency);
}