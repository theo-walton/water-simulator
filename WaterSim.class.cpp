
#include "WaterSim.class.hpp"

std::vector<float>	WaterSim::ParseNumbers(std::string str)
{
	std::stringstream ss;
	std::vector<float> out;

	ss << str;

	std::string temp;
	float found;
	while (!ss.eof())
	{
		ss >> temp;

		if (std::stringstream(temp) >> found)
			out.push_back(found);
		temp = "";
	}
	return out;
}

std::string	WaterSim::ReadFile(std::string filepath)
{
	std::ifstream f(filepath);
	std::stringstream buf;

	buf << f.rdbuf();
	return buf.str();
}

void	WaterSim::AddLandCoordinates(std::vector<float> rawValues)
{
	for (int i = 2; i < rawValues.size(); i+=3)
	{
		glm::vec3 p = {rawValues[i - 2], rawValues[i - 1], rawValues[i]};
		_landPoints.push_back(p);
	}	
}

float	WaterSim::GetHeight(int x, int y)
{	
	std::vector<float> heightWeights;
	constexpr float d = 0.001;
	constexpr float p = 2;
	
	for (int i = 0; i < _landPoints.size(); i++)
	{
		float xdif = _landPoints[i].x - x;
		float ydif = _landPoints[i].y - y;
		float dist = fmax( sqrt(xdif * xdif + ydif * ydif), d );

		heightWeights.push_back(1 / pow(dist, p));
	}
	float distToSides[4] = { static_cast<float>(fmax(x, d)),
				 static_cast<float>(fmax(Map::size - x - 1, d)),
				 static_cast<float>(fmax(y, d)),
				 static_cast<float>(fmax(Map::size - y - 1, d)) };

	for (int i = 0; i < 4; i++)
		heightWeights.push_back(1 / pow(distToSides[i], p));

	float weightedHeightSum = 0;
	float weightSum = 0;

	for (int i = 0; i < _landPoints.size(); i++)
	{
		weightedHeightSum += _landPoints[i].z * heightWeights[i];
		weightSum += heightWeights[i];
	}
	for (int i = 0; i < 4; i++)
		weightSum += heightWeights[_landPoints.size() + i];

	return weightedHeightSum / weightSum;
}

void	WaterSim::InterpolatePoints(void)
{
	for (int y = 0; y < Map::size; y++)
	{
		for (int x = 0; x < Map::size; x++)
		{
			_landHeights.push_back(GetHeight(x, y));
		}
	}
}

void	WaterSim::NormalizePointLocations(float size)
{	
	glm::vec3 max = {size, size, size};
	glm::vec3 min = {0, 0, 0};

	for (int i = 0; i < _landPoints.size(); i++)
	{
		if (max.x < _landPoints[i].x)
			max.x = _landPoints[i].x;
		if (max.y < _landPoints[i].y)
			max.y = _landPoints[i].y;
		if (max.z < _landPoints[i].z)
			max.z = _landPoints[i].z;

		if (min.x > _landPoints[i].x)
			min.x = _landPoints[i].x;
		if (min.y > _landPoints[i].y)
			min.y = _landPoints[i].y;
		if (min.z > _landPoints[i].z)
			min.z = _landPoints[i].z;
	}

	glm::vec3 scale = (size - 2 * mapBorder) / (max - min);

	for (int i = 0; i < _landPoints.size(); i++)
	{
		_landPoints[i] = (_landPoints[i] - min) * scale +
			glm::vec3(mapBorder, mapBorder, mapBorder);
	}
}

glm::vec3	WaterSim::GetCoordinate(int x, int y)
{
	int index = x + y * Map::size;

	return glm::vec3(x, y, _landHeights[index]);
}

glm::vec3	WaterSim::GetNormal(int x, int y)
{	
	float Az = (x + 1 < Map::size) ? _landHeights[(x + 1) + y * Map::size] : 0;
	float Bz = (y + 1 < Map::size) ? _landHeights[x + (y + 1) * Map::size] : 0;
	float Cz = (x > 0) ? _landHeights[(x - 1) + y * Map::size] : 0;
	float Dz = (y > 0) ? _landHeights[x + (y - 1) * Map::size] : 0;

	return glm::normalize(glm::vec3(Cz - Az, Dz - Bz, 2));
}

glm::vec3	WaterSim::GetColor(int x, int y)
{
	int index = x + y * Map::size;

	return glm::vec3(_landHeights[index] / 100 , 0.4, 0.1);
}

void	WaterSim::GetLandArrays(void)
{
	for (int y = 0; y < Map::size; y++)
	{
		for (int x = 0; x < Map::size; x++)
		{
			glm::vec3 coor = GetCoordinate(x, y);
			glm::vec3 normal = GetNormal(x, y);
			glm::vec3 color = GetColor(x, y);

			_landVertices.insert(_landVertices.end(), &coor[0], &coor[0] + 3);
			_landNormals.insert(_landNormals.end(), &normal[0], &normal[0] + 3);
			_landColors.insert(_landColors.end(), &color[0], &color[0] + 3);
		}
	}

	for (int y = 0; y < Map::size - 1; y++)
	{
		for (int x = 0; x < Map::size - 1; x++)
		{
			GLuint index = x + y * Map::size;
			GLuint quadIndices[6] = { index,
						 index + Map::size,
						 index + Map::size + 1,
						 index,
						 index + Map::size + 1,
						 index + 1 };
			
			_landIndices.insert(_landIndices.end(), &quadIndices[0], &quadIndices[0] + 6);
		}
	}
}

void	WaterSim::SendLandArraysToOpenGL(void)
{
	glGenBuffers(1, &_landIndicesID);
	glGenBuffers(1, &_landVerticesID);
	glGenBuffers(1, &_landNormalsID);
	glGenBuffers(1, &_landColorsID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _landIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		     sizeof(GLuint) * _landIndices.size(),
		     &_landIndices[0],
		     GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, _landVerticesID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _landVertices.size(),
		     &_landVertices[0],
		     GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, _landNormalsID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _landNormals.size(),
		     &_landNormals[0],
		     GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, _landColorsID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _landColors.size(),
		     &_landColors[0],
		     GL_STATIC_DRAW);
}

float	WaterSim::GetMapTotalHeight(int x, int y)
{
	return _map->GetMap()[x + y * Map::size].waterHeight +
	       _map->GetMap()[x + y * Map::size].landHeight;
}

glm::vec3	WaterSim::GetWaterCoordinateAndHeight(int x, int y, float &height)
{
	if (_map->GetMap()[x + y * Map::size].waterHeight != 0)
	{
		height = _map->GetMap()[x + y * Map::size].waterHeight;
		return glm::vec3(x, y, GetMapTotalHeight(x, y));		
	}

	float nearbyHeight = 0;
	float nearbyWaterHeight = 0;
	int nearbyWater = 0;
	
	for (int xiter = x - 1; xiter <= x + 1; xiter++)
	{
		for (int yiter = y - 1; yiter <= y + 1; yiter++)
		{
			if ( xiter >= 0 && xiter < Map::size && yiter >= 0 && yiter < Map::size &&
			     _map->GetMap()[xiter + yiter * Map::size].waterHeight != 0)
			{
				nearbyHeight += GetMapTotalHeight(xiter, yiter);
				nearbyWaterHeight +=
					_map->GetMap()[xiter + yiter * Map::size].waterHeight;
				nearbyWater++;
			}
		}
	}
	if (nearbyWater != 0)
	{
		height = nearbyWaterHeight / nearbyWater;
		return glm::vec3(x, y, nearbyHeight / nearbyWater);
	}
	height = 0;
	return glm::vec3(x, y, 0);
}

glm::vec3	WaterSim::GetWaterNormal(int x, int y)
{
	float Az = (x + 1 < Map::size) ? GetMapTotalHeight(x + 1, y) : 0;
	float Bz = (y + 1 < Map::size) ? GetMapTotalHeight(x, y + 1) : 0;
	float Cz = (x > 0) ? GetMapTotalHeight(x - 1, y) : 0;
	float Dz = (y > 0) ? GetMapTotalHeight(x, y - 1) : 0;
       
	return glm::normalize(glm::vec3(Cz - Az, Dz - Bz, 2));
}

void	WaterSim::GetWaterArrays(void)
{
	_waterVertices.clear();
	_waterNormals.clear();
	_waterHeights.clear();
	for (int y = 0; y < Map::size; y++)
	{
		for (int x = 0; x < Map::size; x++)
		{
			float height;
			glm::vec3 coor = GetWaterCoordinateAndHeight(x, y, height);
			glm::vec3 normal = GetWaterNormal(x, y);

			_waterVertices.insert(_waterVertices.end(), &coor[0], &coor[0] + 3);
			_waterNormals.insert(_waterNormals.end(), &normal[0], &normal[0] + 3);
			_waterHeights.push_back(height);
		}
	}
}

void	WaterSim::SendWaterArraysToOpenGL(void)
{
        glDeleteBuffers(1, &_waterVerticesID);
        glDeleteBuffers(1, &_waterNormalsID);
        glDeleteBuffers(1, &_waterHeightsID);
	
	glGenBuffers(1, &_waterVerticesID);
	glGenBuffers(1, &_waterNormalsID);
	glGenBuffers(1, &_waterHeightsID);

	glBindBuffer(GL_ARRAY_BUFFER, _waterVerticesID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _waterVertices.size(),
		     &_waterVertices[0],
		     GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, _waterNormalsID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _waterNormals.size(),
		     &_waterNormals[0],
		     GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, _waterHeightsID);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof(GLfloat) * _waterHeights.size(),
		     &_waterHeights[0],
		     GL_STATIC_DRAW);
}

void	WaterSim::initWaterSheet(void)
{
	for (int i = 0; i < _landHeights.size(); i++)
	{
		_waterSheetTest.push_back(20);
	}
}

void	WaterSim::InitWaveSheet(void)
{
	for (int y = 0; y < Map::size; y++)
	{
		for (int x = 0; x < Map::size; x++)
		{
			if (x == 1)
				_waveSheet.push_back(10);
			else
				_waveSheet.push_back(0);
		}
	}
}

void	WaterSim::InitRainSheet(void)
{
        for (int i = 0; i < _landHeights.size(); i++)
        {
                _rainSheet.push_back(0);
        }
}

void	WaterSim::InitDrainSheet(void)
{
	for (int i = 0; i < _landHeights.size(); i++)
	{
		_drainSheet.push_back(-0.05);
	}
}

void	WaterSim::InitSinkSheet(void)
{
        for (int y = 0; y < Map::size; y++)
        {
                for (int x = 0; x < Map::size; x++)
                {
			if (x > 45 && y > 45 && x < 55 && y < 55)
				_sinkSheet.push_back(-10);
			else
				_sinkSheet.push_back(0);
                }
        }
}

void	WaterSim::InitFloodSheet(void)
{
	for (int i = 0; i < _landHeights.size(); i++)
	{
		_floodSheet.push_back(0.1);
	}
}

void	WaterSim::InitRisingLevelSheet(void)
{
	for (int i = 0; i < _landHeights.size(); i++)
	{
		_risingLevelSheet.push_back(0);
	}
}

WaterSim::WaterSim(std::string filepath)
{
	_landProgram = new ShadingProgram("landVertex.glsl", "", "landFrag.glsl");
	_waterProgram = new ShadingProgram("waterVertex.glsl", "", "waterFrag.glsl");
	_waterPerspectiveID = glGetUniformLocation(_waterProgram->ID(), "perspective");
	_landPerspectiveID = glGetUniformLocation(_landProgram->ID(), "perspective");
	
	_landProgram->Use();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	AddLandCoordinates(ParseNumbers(ReadFile(filepath)));
	
	// put all points in range 0 <= x, y, z <= Map::size - mapBorder
	NormalizePointLocations(Map::size);
	
	InterpolatePoints();
	_map = new Map(_landHeights);
        initWaterSheet();
	InitWaveSheet();
	InitRainSheet();
	InitDrainSheet();
	InitSinkSheet();
	InitFloodSheet();
	InitRisingLevelSheet();
//	_map->AddWater(_waterSheetTest);
	GetLandArrays();
	GetWaterArrays();

	SendLandArraysToOpenGL();
	SendWaterArraysToOpenGL();
}

void	WaterSim::WaterIteration(void)
{
	_map->WaterFlowIteration();
	GetWaterArrays();
	SendWaterArraysToOpenGL();
}

void	WaterSim::WaveIteration(void)
{
	_map->AddWater(_waveSheet);
}

void	WaterSim::RainIteration(void)
{
	constexpr float rainBorder = 30;
	
	for (int x = 0; x < Map::size; x++)
	{
		for (int y = 0; y < Map::size; y++)
		{
			if (x > rainBorder && x < Map::size - rainBorder &&
			    y > rainBorder && y < Map::size - rainBorder)
			{
				_rainSheet[x + y * Map::size] =
					(rand() % 100 >= 99) ? 2 : 0;
			}
			else
				_rainSheet[x + y * Map::size] = 0;
		}		
	}
	_map->AddWater(_rainSheet);
}

void	WaterSim::DrainIteration(void)
{
	_map->AddWater(_drainSheet);
}

void	WaterSim::SinkIteration(void)
{
	_map->AddWater(_sinkSheet);
}

void	WaterSim::FloodIteration(void)
{
	_map->AddWater(_floodSheet);
}

void	WaterSim::RisingLevelIteration(float h)
{
	for (int x = 0; x < Map::size; x++)
	{
		for (int y = 0; y < Map::size; y++)
		{
			float diff = h - GetMapTotalHeight(x, y);

			if (diff > 0)
				_risingLevelSheet[x + y * Map::size] = diff;
			else
				_risingLevelSheet[x + y * Map::size] = 0;
		}
	}
	_map->AddWater(_risingLevelSheet);
}

void	WaterSim::NewPerspective(glm::mat4 m)
{
	_landProgram->Use();
	glUniformMatrix4fv(_landPerspectiveID,
			   1,
			   GL_FALSE,
			   glm::value_ptr(m));
	_waterProgram->Use();
	glUniformMatrix4fv(_waterPerspectiveID,
			   1,
			   GL_FALSE,
			   glm::value_ptr(m));
}

void	WaterSim::DrawLand(void)
{
	_landProgram->Use();

	glDisable(GL_BLEND);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _landVerticesID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _landNormalsID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _landColorsID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _landIndicesID);

	glDrawElements(GL_TRIANGLES,
		       _landIndices.size(),
		       GL_UNSIGNED_INT,
		       0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void	WaterSim::DrawWater(void)
{
	_waterProgram->Use();
	
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _waterVerticesID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _waterNormalsID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _waterHeightsID);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _landIndicesID);

	glDrawElements(GL_TRIANGLES,
		       _landIndices.size(),
		       GL_UNSIGNED_INT,
		       0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void	WaterSim::printLand(void)
{
	for (int y = 0; y < Map::size; y++)
	{
		for (int x = 0; x < Map::size; x++)
		{
			std::cout << std::setw(2) << (int)_landHeights[x + y * Map::size];
			std::cout << " ";
		}
		std::cout << std::endl;
	}
}
