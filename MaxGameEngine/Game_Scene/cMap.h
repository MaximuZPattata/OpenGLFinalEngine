#pragma once
#include <cControlGameEngine.h>

class cMap
{
public:
	std::vector<int> mazeVector;
	std::vector<int> treasurePosIndexList;
	std::vector<glm::vec3> maze3DPosList;

	int NUM_OF_ROWS_IN_MAZE = 149;
	int NUM_OF_COLUMNS_IN_MAZE = 115;

	float drawWithinThisRange = 750.f;

	bool InitializeMap(cControlGameEngine& gameEngine);
	void Load3DMaze(cControlGameEngine& gameEngine);
	void LoadWallModel(cControlGameEngine& gameEngine, glm::vec3 modelPos, float index);
	void LoadFloorModel(cControlGameEngine& gameEngine, glm::vec3 modelPos, float index);
	//void LoadTreasuresRandomly(int totalTreasures);
	bool LoadMapFromFile(std::string fileName);
};

