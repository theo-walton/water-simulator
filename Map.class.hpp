#ifndef MAP_CLASS_HPP
#define MAP_CLASS_HPP

#include <vector>
#include <algorithm>

class	Map
{
	public:	struct MapBlock
	{
		float waterHeight;
		float landHeight;
	};
	
	public: struct MapBlockReferal
	{
		MapBlock *block;
		int index;

		bool	operator < (const MapBlockReferal &obj) const;
	};
	
private:
	
	std::vector<MapBlock> _blocks;
	std::vector<MapBlockReferal> _orderedBlocks;

	void	UpdateWaterLevel(int blockIndex);
	void	DistributeWater(std::vector<MapBlock*> &blocks, float water);

public:

	Map(std::vector<float> &landHeights);

	static constexpr int size = 100;

	static bool     OutOfBounds(int x, int y);
	static float	BlockHeight(MapBlock*);
	
	void	AddWater(std::vector<float> &waterHeights);
	void	WaterFlowIteration(void);
	std::vector<MapBlock>	&GetMap(void);	
};

#endif
