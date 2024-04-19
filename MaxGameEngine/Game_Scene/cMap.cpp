#include "pch.h"
#include "cMap.h"

unsigned int RandomNumberGenerator(unsigned int maxSize)
{
	// Seeding with a random device
	std::random_device rd;
	std::mt19937 gen(rd());

	// Fixing the random index range
	std::uniform_int_distribution<> dist(0, maxSize);

	return dist(gen);
}

bool cMap::InitializeMap(cControlGameEngine& gameEngine)
{
	if (!LoadMapFromFile("DungeonFile/MyMaze.txt"))
		return false;

	this->Load3DMaze(gameEngine);
	//this->LoadTreasuresRandomly(250);

	return true;
}

bool cMap::LoadMapFromFile(std::string fileName) // Loading the maze from the text file
{
	std::ifstream mazeFile(fileName);

	if (!mazeFile.is_open())
	{
		std::cout << std::endl;
		std::cout << "ERROR : UNABLE TO OPEN MAZE FILE" << std::endl;

		return false;
	}

	char singleCharacter;
	int characterCount = 0;

	while (mazeFile.get(singleCharacter))
	{
		if (singleCharacter == 'X')
			mazeVector.push_back(1);

		else if (singleCharacter == '.')
			mazeVector.push_back(0);

		characterCount++;
	}

	std::cout << std::endl;
	std::cout << "SUCCESS : MAZE FILE READ" << std::endl;

	return true;
}

// Load Wall Model Details
void cMap::LoadWallModel(cControlGameEngine& gameEngine, glm::vec3 modelPos, float index)
{
	float yPosOffset = 18.f;

	std::vector <std::string> modelFilePathList;
	std::vector <float> minimumDistanceList;
	std::vector <bool> defaultModelBooleanList;

	std::string	modelName = "Wall_" + std::to_string(index);
	std::string modelFileName = "Brick.ply";
	/*std::string modelTextureFileName = "Brick.bmp";
	std::string modelTextureName = "BrickWallTexture";*/

	modelFilePathList.push_back(modelFileName);
	minimumDistanceList.push_back(this->drawWithinThisRange);
	defaultModelBooleanList.push_back(true);

	gameEngine.LoadModelsInto3DSpace(modelFileName, modelName, modelPos.x, modelPos.y + yPosOffset, modelPos.z);
	gameEngine.ScaleModel(modelName, 117.5f);
	gameEngine.SetMeshSceneId(modelName, 0);
	//gameEngine.TurnMeshLightsOn(modelName);
	//gameEngine.UseTextures(modelName, true);
	//gameEngine.AddTexturesToOverlap(modelName, modelTextureFileName, modelTextureName);
	gameEngine.AddLODToMesh(modelName, modelFilePathList, minimumDistanceList, defaultModelBooleanList);
}

// Load Floor Model details
void cMap::LoadFloorModel(cControlGameEngine& gameEngine, glm::vec3 modelPos, float index)
{
	float yPosOffset = 1.f;
	float zPosOffset = 18.f;

	std::vector <std::string> modelFilePathList;
	std::vector <float> minimumDistanceList;
	std::vector <bool> defaultModelBooleanList;

	std::string	modelName = "Floor_" + std::to_string(index);
	std::string modelFileName = "Floor.ply";
	std::string modelTextureFileName = "Dungeons_2_Texture_01_A.bmp";
	std::string modelTextureName = "FloorTexture";

	modelFilePathList.push_back(modelFileName);
	minimumDistanceList.push_back(this->drawWithinThisRange);
	defaultModelBooleanList.push_back(true);

	gameEngine.LoadModelsInto3DSpace(modelFileName, modelName, modelPos.x, modelPos.y + yPosOffset, modelPos.z + zPosOffset);
	gameEngine.ScaleModel(modelName, 0.07);
	gameEngine.SetMeshSceneId(modelName, 0);
	gameEngine.TurnMeshLightsOn(modelName);
	gameEngine.UseTextures(modelName, true);
	gameEngine.AddTexturesToOverlap(modelName, modelTextureFileName, modelTextureName);
	gameEngine.AddLODToMesh(modelName, modelFilePathList, minimumDistanceList, defaultModelBooleanList);
}

void cMap::Load3DMaze(cControlGameEngine& gameEngine)
{
	float xPos = 0.f;
	float yPos = 0.f;
	float zPos = 0.f;
	float distanceBetweenBlocks = 35.f; // 35 units

	int endOfColumnCounter = 0;

	//-------------------------Scan through the maze vector for walls and free spaces and load them------------------------------

	for (int index = 0; index < mazeVector.size(); index++)
	{
		endOfColumnCounter++;

		maze3DPosList.push_back(glm::vec3(xPos, yPos, zPos));

		if (mazeVector[index] == 1)
		{
			LoadWallModel(gameEngine, maze3DPosList[index], index);
			xPos += distanceBetweenBlocks;
		}
		else if (mazeVector[index] == 0)
		{
			xPos += distanceBetweenBlocks;
			LoadFloorModel(gameEngine, maze3DPosList[index], index);
		}

		if (endOfColumnCounter == NUM_OF_COLUMNS_IN_MAZE)
		{
			xPos = 0;
			zPos += distanceBetweenBlocks;
			endOfColumnCounter = 0;
		}
	}

	std::cout << std::endl;
	std::cout << "LOADED 3D MAZE" << std::endl;
}
//
//void LoadTreasuresRandomly(int totalTreasures)
//{
//	std::string modelName;
//
//	bool newRandomNumGenerated = false;
//
//	unsigned int randomNumIndex = 0;
//
//	int LeftOfIndex = 0;
//	int RightOfIndex = 0;
//	int TopOfIndex = 0;
//	int BottomOfIndex = 0;
//
//	std::cout << std::endl;
//
//	for (int treasureCount = 1; treasureCount <= totalTreasures; treasureCount++)
//	{
//		randomNumIndex = RandomNumberGenerator(mazeVector.size() - 1);
//		newRandomNumGenerated = false;
//
//		while (!newRandomNumGenerated)
//		{
//			newRandomNumGenerated = true;
//
//			//-----------------To check if there are any treasures already in that location-------------------------
//
//			for (int treasureIndex = 0; treasureIndex < treasurePosIndexList.size(); treasureIndex++)
//			{
//				if (randomNumIndex == treasurePosIndexList[treasureIndex])
//					newRandomNumGenerated = false;
//			}
//
//			//-----------------------To check if there are any walls in that location-------------------------------
//
//			if (mazeVector[randomNumIndex] != 0)
//				newRandomNumGenerated = false;
//
//			//-------------------To check if there are any treasures adjacent to this one--------------------------
//
//			if (newRandomNumGenerated)
//			{
//				if (randomNumIndex - 1 > 0)
//				{
//					if (maze3DPosList[randomNumIndex - 1].z == maze3DPosList[randomNumIndex].z)
//						LeftOfIndex = randomNumIndex - 1;
//					else
//						LeftOfIndex = 0;
//				}
//
//				if (randomNumIndex + 1 < NUM_OF_ROWS_IN_MAZE * NUM_OF_COLUMNS_IN_MAZE)
//				{
//					if (maze3DPosList[randomNumIndex + 1].z == maze3DPosList[randomNumIndex].z)
//						RightOfIndex = randomNumIndex + 1;
//					else
//						RightOfIndex = 0;
//				}
//
//				if (randomNumIndex - NUM_OF_COLUMNS_IN_MAZE > NUM_OF_COLUMNS_IN_MAZE)
//					TopOfIndex = randomNumIndex - NUM_OF_COLUMNS_IN_MAZE;
//				else
//					TopOfIndex = 0;
//
//				if (randomNumIndex + NUM_OF_COLUMNS_IN_MAZE < NUM_OF_ROWS_IN_MAZE * NUM_OF_COLUMNS_IN_MAZE)
//					BottomOfIndex = randomNumIndex + NUM_OF_COLUMNS_IN_MAZE;
//				else
//					BottomOfIndex = 0;
//
//				for (int treasureIndex = 0; treasureIndex < treasurePosIndexList.size(); treasureIndex++)
//				{
//					if (treasurePosIndexList[treasureIndex] == LeftOfIndex || treasurePosIndexList[treasureIndex] == RightOfIndex
//						|| treasurePosIndexList[treasureIndex] == TopOfIndex || treasurePosIndexList[treasureIndex] == BottomOfIndex)
//						newRandomNumGenerated = false;
//				}
//			}
//
//			//----------------------------Generating another random number again-----------------------------------
//
//			if (!newRandomNumGenerated)
//				randomNumIndex = RandomNumberGenerator(mazeVector.size() - 1);
//		}
//
//		modelName = "Treasure_" + std::to_string(randomNumIndex);
//
//		gameEngine.LoadModelsInto3DSpace("Sphere_1_unit_Radius_uv.ply", modelName, maze3DPosList[randomNumIndex].x, 0.5f, maze3DPosList[randomNumIndex].z);
//		gameEngine.ScaleModel(modelName, 0.4f);
//		gameEngine.UseManualColors(modelName, true);
//		gameEngine.ChangeColor(modelName, 0.9, 0.9, 0.0);
//
//		std::cout << "Treasure - " << treasureCount << " | Name - " << modelName
//			<< " | Pos - (" << maze3DPosList[randomNumIndex].x << "," << maze3DPosList[randomNumIndex].y << "," << maze3DPosList[randomNumIndex].z << ")" << std::endl;
//
//		treasurePosIndexList.push_back(randomNumIndex);
//	}
//
//	std::cout << std::endl;
//	std::cout << "LOADED ALL TREASURES" << std::endl;
//}
