#ifndef WATERSIM_CLASS_HPP
#define WATERSIM_CLASS_HPP

#include "GLheaders.hpp"
#include "Map.class.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

class	WaterSim
{
private:

	Map *_map;
       
	ShadingProgram *_landProgram;
	ShadingProgram *_waterProgram;
	
	std::vector<float> _landHeights;
	std::vector<glm::vec3> _landPoints;
	
	std::vector<GLuint> _landIndices;
	std::vector<GLfloat> _landVertices;
	std::vector<GLfloat> _landNormals;
	std::vector<GLfloat> _landColors;

	GLuint _landIndicesID;
	GLuint _landVerticesID;
	GLuint _landNormalsID;
	GLuint _landColorsID;
	GLuint _landPerspectiveID;	

	std::vector<GLfloat> _waterVertices;
	std::vector<GLfloat> _waterNormals;
	std::vector<GLfloat> _waterHeights;

	GLuint _waterVerticesID = 0;
	GLuint _waterNormalsID = 0;
	GLuint _waterHeightsID = 0;
	GLuint _waterPerspectiveID = 0;

	std::vector<float> _waveSheet;
	std::vector<float> _rainSheet;
	std::vector<float> _drainSheet;
	std::vector<float> _sinkSheet;
	std::vector<float> _floodSheet;
	std::vector<float> _risingLevelSheet;
	std::vector<float> _waterSheetTest;
	
	std::vector<float>	ParseNumbers(std::string str);
	std::string	ReadFile(std::string filepath);

	void	initWaterSheet(void);
	void	InitWaveSheet(void);	
	void	InitRainSheet(void);
	void	InitDrainSheet(void);
	void	InitSinkSheet(void);
	void	InitFloodSheet(void);
	void	InitRisingLevelSheet(void);
	
	void	AddLandCoordinates(std::vector<float> rawValues);
	float	GetHeight(int x, int y);
	void	InterpolatePoints(void);
	void	NormalizePointLocations(float size);
	
	glm::vec3	GetCoordinate(int x, int y);
	glm::vec3	GetNormal(int x, int y);
	glm::vec3	GetColor(int x, int y);
	void	GetLandArrays(void);
	void	SendLandArraysToOpenGL(void);

	glm::vec3	GetWaterCoordinateAndHeight(int x, int y, float &height);
	glm::vec3	GetWaterNormal(int x, int y);
	void	GetWaterArrays(void);
	void	SendWaterArraysToOpenGL(void);
	
	float	GetMapTotalHeight(int x, int y);

	
public:

	WaterSim(std::string filepath);

	static constexpr float mapBorder = 10.0f;

	void	NewPerspective(glm::mat4);
	void	DrawLand(void);
	void	DrawWater(void);
	void	WaterIteration(void);
	void	WaveIteration(void);
	void	RainIteration(void);
	void	DrainIteration(void);
	void	SinkIteration(void);
	void	FloodIteration(void);
	void	RisingLevelIteration(float h);
	void	printLand(void);	
	
};

#endif
