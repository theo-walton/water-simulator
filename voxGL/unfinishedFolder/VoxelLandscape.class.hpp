#ifndef VOXELLANDSCAPE_CLASS_HPP
#define VOXELLANDSCAPE_CLASS_HPP

#include "voxGL.hpp"

class VoxelLandscape
{
private:

	glm::vec3 _center;
	VoxelMap *_map;
	int _seed;
	
public:

	void	UpdateChunks(void);	
	
};

#endif
