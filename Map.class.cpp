
#include "Map.class.hpp"

bool	Map::MapBlockReferal::operator < (const Map::MapBlockReferal &obj) const
{
	if (block->waterHeight + block->landHeight <
	    obj.block->waterHeight + obj.block->landHeight)
	{
		return true;
	}
	return false;
}

bool	Map::OutOfBounds(int x, int y)
{
	if (x >= Map::size || x < 0 || y >= Map::size || y < 0)
		return true;
	return false;
}

float	Map::BlockHeight(Map::MapBlock *block)
{
	return block->waterHeight + block->landHeight;
}

void	Map::UpdateWaterLevel(int blockIndex)
{
	int x = blockIndex % size;
	int y = blockIndex / size;

	float water = _blocks[blockIndex].waterHeight;

	_blocks[blockIndex].waterHeight = 0;

	std::vector<MapBlock*> localBlocks;

	for (int xit = x - 1; xit <= x + 1; xit++)
	{
		for (int yit = y - 1; yit <= y + 1; yit++)
		{
			if (!OutOfBounds(xit, yit))
				localBlocks.push_back( &_blocks[yit * size + xit] );
		}				
	}
	DistributeWater(localBlocks, water);
}

void	Map::DistributeWater(std::vector<MapBlock*> &blocks, float water)
{
	std::vector<MapBlockReferal> sortedBlocks;

	for (int i = 0; i < blocks.size(); i++)
	{
		MapBlockReferal blockReferal = {blocks[i], i};

		sortedBlocks.push_back(blockReferal);
	}
	std::sort(sortedBlocks.begin(), sortedBlocks.end());

	float newHeight = BlockHeight(sortedBlocks[0].block);
	
	for (int i = 0; i < sortedBlocks.size() - 1; i++)
	{
		float heightDif =
			BlockHeight(sortedBlocks[i + 1].block) -
			BlockHeight(sortedBlocks[i].block);

		float waterToFill = (i + 1) * heightDif;

		if (water > waterToFill)
		{
			water -= waterToFill;
			newHeight += heightDif;
		}
		else
		{
			newHeight += water / (i + 1);
			water = 0;
			break;
		}
	}
	if (water != 0)
		newHeight += water / sortedBlocks.size();
	
	for (int i = 0; i < sortedBlocks.size(); i++)
	{
		float heightDif = newHeight - BlockHeight(sortedBlocks[i].block);
		if (heightDif <= 0)
			break;

		sortedBlocks[i].block->waterHeight += heightDif;
	}
}

Map::Map(std::vector<float> &landHeights)
{
	for (int i = 0; i < landHeights.size(); i++)
	{
		MapBlock newBlock = {0, landHeights[i]};
		
		_blocks.push_back(newBlock);
	}
	for (int i = 0; i < _blocks.size(); i++)
	{
		MapBlockReferal newReferal = {&_blocks[i], i};

		_orderedBlocks.push_back(newReferal);
	}
}

void	Map::AddWater(std::vector<float> &waterHeights)
{
	for (int i = 0; i < _blocks.size(); i++)
	{
		_blocks[i].waterHeight += waterHeights[i];
	}
}

void	Map::WaterFlowIteration(void)
{
	std::sort(_orderedBlocks.begin(), _orderedBlocks.end());

	for (int i = 0; i < _orderedBlocks.size(); i++)
	{
		UpdateWaterLevel(_orderedBlocks[i].index);
	}
}

std::vector<Map::MapBlock>	&Map::GetMap(void)
{
	return _blocks;
}
