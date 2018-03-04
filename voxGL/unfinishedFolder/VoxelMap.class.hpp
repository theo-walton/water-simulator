#ifndef VOXELMAP_CLASS_HPP
#define VOXELMAP_CLASS_HPP

#include "voxGL.hpp"

class VoxelMap
{
private:

	static const int size = 1000;
	
	ShadingProgram &_program;
	GLuint _perspectiveID;
	GLuint _transformID;
	glm::mat4 _perspective;
	GLint _numlist[size];
	std::vector<VoxelChunk*> _chunks;

public:

	VoxelMap(ShadingProgram &program);
	
	void	AddChunk(VoxelChunk *chunk);
	void	RemoveChunks(std::function<bool(glm::vec3, glm::vec3)> &f, glm::vec3 pos);
	void	Render(void);
	void	NewPerspective(glm::mat4);
	void	UsePerspective(void);
};

#endif
