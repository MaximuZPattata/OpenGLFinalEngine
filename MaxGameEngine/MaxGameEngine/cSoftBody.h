#pragma once

#include "pch.h"
#include "sModelDrawInfo.h"
#include "cMesh.h"

struct sNode
{
	unsigned int nodeIndex = 0;

	bool bFixedNode = false;

	glm::vec3 currentPosition = glm::vec3(0.0f);
	glm::vec3 oldPosition = glm::vec3(0.0f);

	glm::vec3 initialPosition = glm::vec3(0.0f);

	sVertexWithBone* modelVertexPointer = NULL;
};

class cSoftBody // Verlet
{
public:
	cSoftBody();
	~cSoftBody();

	struct sConstraint
	{
		sConstraint() {}

		sNode* nodeA = NULL;
		sNode* nodeB = NULL;

		float restLength = 0.0f;

		unsigned int iterationsCount = 1;

		bool bIsActive = true;
	};

	bool bEnableDebugSphere = false;
	bool bSoftBodyHandledByThread = false;
	bool bApplyDampingEffect = false;
	bool bChainEffect = false;
	bool bBallEffect = false;
	bool bCubeEffect = false;
	bool bSoftBodyInMotion = true;

	float debugSpheresScaleValue = 0.f;
	float dampingFactor = 1.0f;
	float softBodyScaleValue = 0.f;

	std::string debugModelName = "";

	glm::vec3 acceleration = glm::vec3(0.0f);

	glm::vec4 debugSpheresColor = glm::vec4(0.f);

	std::vector < sNode* > nodesList;
	std::vector < std::string > debugSpheresMeshNameList;

	sModelDrawInfo mModelVertexInfo;

	bool CreateSoftBody(sModelDrawInfo& modelDrawInfo, glm::vec3 softBodyPos, glm::mat4 matInitalTransform = glm::mat4(1.0f));

	void UpdateVertexPositions();

	void UpdateNormals(void);

	void VerletUpdate(double deltaTime);

	void ApplyCollision(double deltaTime, std::vector < cSoftBody* > softBodyCollisionList);

	void SatisfyConstraints(void);

	void CreateRandomBracing(unsigned int numberOfBraces, float minDistanceBetweenVertices);

	void CreateWheelBracing();

	void CreateImaginaryCenterNodeBracing();

	void ApplyDirectionAcceleration(glm::vec3 directedAcceleration);

	void AddBottomConnectorNode(sNode* connectingNode);

	void AddTopConnectorNode(sNode* connectingNode);

	void CreateManualBracing(sNode* nodeA, sNode* nodeB);

	sNode* GetBottomNode();

	sNode* GetTopNode();

	glm::vec3 GetCurrentPos();

private:
	const double MAX_DELTATIME = 1.0 / 60.0;

	std::vector < sConstraint* > mConstraintsList;
	std::vector < sNode* > mTopNodesList;
	std::vector < sNode* > mBottomNodesList;

	sNode* mImaginaryCenterNode = NULL;
	sNode* mTopConnectorNode = NULL;
	sNode* mBottomConnectorNode = NULL;

	void AddImaginaryCenterNode(glm::vec3 softBodyPos);

	void FindCenterNodes();

	void LoadTopMostNode();

	void LoadBottomMostNode();

	void CalculateDampingValue(float deltaTime);

	void CleanZeros(glm::vec3& value);

	float CalculateDistance(sNode* nodeA, sNode* nodeB);
};
