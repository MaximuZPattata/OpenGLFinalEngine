#include "pch.h"
#include "cJsonWriter.h"

void cJsonWriter::WriteDataToTextFile(cControlGameEngine& gameEngine)
{
    std::ofstream outputFile("ModelData.txt");

    if (!outputFile.is_open())
        std::cerr << "ERROR : FAILED TO OPEN FILE TO WRITE" << std::endl;

    std::vector <cMesh* > totalMeshList = gameEngine.GetMeshList();

    outputFile << "ALL MESH POSITIONS :" << std::endl << std::endl;

    for (cMesh* currentMesh : totalMeshList)
        outputFile << currentMesh->meshUniqueName << " : (" << currentMesh->drawPosition.x << ", " << currentMesh->drawPosition.y << ", "
        << currentMesh->drawPosition.z << ")" << std::endl << std::endl;

    outputFile.close();
}
