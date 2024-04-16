#pragma once
#include "pch.h"
#include "cShaderManager.h"
#include "cVAOManager.h"
#include "cMesh.h"

class cDebugRenderer
{
public:
	GLuint debugShaderProgramID = 0;

	cDebugRenderer();

	bool Initialize(cShaderManager* mDebugRendererShaderManager, cVAOManager* mDebugRendererMeshManager, GLuint& cDebugRenderer_vertex_buffer, GLuint& cDebugRenderer_VAO);
	
	void CreateNewDebugSpheres(cMesh* newSphereModel, std::vector <glm::vec3> positionsList);

	void UpdateDebugSpherePositions(std::string debugSphereName, std::vector <glm::vec3> positionsList);

	std::vector<glm::vec3> GetSphereModelPositions(std::string debugSphereMeshName);

	//std::string getLastError(void);

	//void ShutDown(cShaderManager* mDebugRendererShaderManager);
	//void RenderDebugObjects(double deltaTime, glm::mat4 matView, glm::mat4 matProjection, cShaderManager* mDebugRendererShaderManager, GLuint& cDebugRenderer_VAO);
	//void AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA);

	
private:
	std::string mLastError;

	struct sDebugSphereDetails
	{
		cMesh* sphereModel = NULL;

		std::vector <glm::vec3> spherePositionsList;
	};

	struct sLineVertexBuffer
	{
		float x, y, z, w;
		float r, g, b, a;
	};

	//struct sSphere
	//{
	//	float radius;

	//	glm::vec3 centreXYZ;

	//	glm::vec4 RGBA;
	//};

	//struct sCube
	//{
	//	float size;

	//	glm::vec3 centreXYZ;

	//	glm::vec4 RGBA;
	//};

	const unsigned int LINE_VERTEX_BUFFER_SIZE = 20'000;

	unsigned int mSizeOfLineVertexBuffer;
	
	sLineVertexBuffer* mLinesVerticesToDraw;

	//std::vector<sSphere>	mSpheresToDrawList;
	//std::vector<sCube>		mCubesToDrawList;
	std::vector<sDebugSphereDetails*> mDebugSphereDetailsList;
};
