
#include "voxGL.hpp"

VoxelMap::VoxelMap(ShadingProgram &program) : _program(program)
{
	_perspectiveID = glGetUniformLocation(program.ID(), "perspective");
	_transformID = glGetUniformLocation(program.ID(), "transform");

	for (int i = 0; i < 1000; i++)
	{
		_numlist[i] = i;
	}
}

void	VoxelMap::AddChunk(VoxelChunk *chunk)
{
	_chunks.push_back(chunk);	
}

void	VoxelMap::RemoveChunks(std::function<bool(glm::vec3, glm::vec3)> &f, glm::vec3 v)
{
	for (int i = 0; i < _chunks.size(); i++)
	{
		if ( f(_chunks[i]->Pos(), v))
		{
			_chunks[i]->Unload();
			_chunks.erase(_chunks.begin() + i);
		}
	}
}

void	VoxelMap::Render(void)
{
	GLuint id;
	
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLint) * 1000,
		     _numlist,
		     GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 0, 0);
	
	for (int i = 0; i < _chunks.size(); i++)
	{
		glUniformMatrix4fv(_transformID,
				   1,
				   GL_FALSE,
				   glm::value_ptr(_chunks[i]->GetTransform()));
		_chunks[i]->Render();
	}

	glDisableVertexAttribArray(1);
}

void	VoxelMap::NewPerspective(glm::mat4 m)
{
	_perspective = m;
}

void	VoxelMap::UsePerspective(void)
{
	glUniformMatrix4fv(_perspectiveID,
			   1,
			   GL_FALSE,
			   glm::value_ptr(_perspective));	
}
