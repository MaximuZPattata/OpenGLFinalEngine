#include "pch.h"
#include "cSoftBody.h"

cSoftBody::cSoftBody() {}

cSoftBody::~cSoftBody() {}

bool CompareAscendingNodeYPosition(const sNode* nodeA, const sNode* nodeB)
{
	return nodeA->currentPosition.y > nodeB->currentPosition.y;
}

bool CompareDescendingNodeYPosition(const sNode* nodeA, const sNode* nodeB)
{
	return nodeA->currentPosition.y < nodeB->currentPosition.y;
}

void cSoftBody::AddImaginaryCenterNode(glm::vec3 softBodyPos)
{
	//---------------------------------Initializing Imaginary center node------------------------------------------------------

	this->mImaginaryCenterNode = new sNode();
	this->mImaginaryCenterNode->currentPosition = this->mImaginaryCenterNode->oldPosition = softBodyPos;
	this->mImaginaryCenterNode->nodeIndex = this->nodesList.size();

	this->CleanZeros(this->mImaginaryCenterNode->currentPosition);
	this->CleanZeros(this->mImaginaryCenterNode->oldPosition);
}

void cSoftBody::FindCenterNodes()
{
	//----------------Adding all the vertex's position and taking the average to get the centre position-----------------------

	//glm::vec3 centerPos(0.f);

	//for (sNode* vertex : this->nodesList)
	//	centerPos += vertex->currentPosition;

	//centerPos /= static_cast<float>(this->nodesList.size());

	////---------------------------Checking which vertices are closer to the center point----------------------------------------

	//float minDistance = 0.f;
	//float tempDistance = 0.f;

	//sNode* centerNode = NULL;

	//for (int index = 0; index < this->nodesList.size(); index++)
	//{
	//	tempDistance = glm::length(this->nodesList[index]->currentPosition - centerPos);

	//	if (index == 0)
	//		minDistance = tempDistance;
	//	else
	//	{
	//		if (tempDistance < minDistance)
	//		{
	//			this->centerNodesList.clear();

	//			minDistance = tempDistance;
	//			centerNode = this->nodesList[index];
	//			//printf("\nMin Dist : %f | Node Pos : (%f, %f, %f)", minDistance, centerNode->currentPosition.x, centerNode->currentPosition.y, centerNode->currentPosition.z);

	//			this->centerNodesList.push_back(centerNode);
	//		}
	//		else if (tempDistance == minDistance)
	//			this->centerNodesList.push_back(centerNode);
	//	}
	//}

	return;
}

bool cSoftBody::CreateSoftBody(sModelDrawInfo& modelInfo, glm::vec3 softBodyPos, glm::mat4 matInitalTransform)
{
	this->mModelVertexInfo = modelInfo;

	//--------Copying Vertices to a local copy ----------------------

	sVertexWithBone* localVertices = new sVertexWithBone[modelInfo.numberOfVertices];

	//std::cout << "Local Vertices = { ";

	for (unsigned int index = 0; index != modelInfo.numberOfVertices; index++)
	{
		glm::vec4 theVertex = glm::vec4(modelInfo.pVertices[index].x, modelInfo.pVertices[index].y, modelInfo.pVertices[index].z, 1.0f);

		theVertex = matInitalTransform * theVertex;

		localVertices[index].x = theVertex.x;
		localVertices[index].y = theVertex.y;
		localVertices[index].z = theVertex.z;

		localVertices[index].w = modelInfo.pVertices[index].w;

		localVertices[index].r = modelInfo.pVertices[index].r;
		localVertices[index].g = modelInfo.pVertices[index].g;
		localVertices[index].b = modelInfo.pVertices[index].b;
		localVertices[index].a = modelInfo.pVertices[index].a;

		localVertices[index].nx = modelInfo.pVertices[index].nx;
		localVertices[index].ny = modelInfo.pVertices[index].ny;
		localVertices[index].nz = modelInfo.pVertices[index].nz;
		localVertices[index].nw = modelInfo.pVertices[index].nw;

		localVertices[index].u = modelInfo.pVertices[index].u;
		localVertices[index].v = modelInfo.pVertices[index].v;

		//std::cout << "| Vertex #" << index << " : (" << localVertices[index].x << ", " << localVertices[index].y << ", " << localVertices[index].z << ") |";
	}

	/*std::cout << " }" << std::endl;
	std::cout << std::endl;*/

	//----------------------Copying Indices to a local copy--------------------------------------

	unsigned int* localIndices = new unsigned int[modelInfo.numberOfIndices];

	for (unsigned int index = 0; index != modelInfo.numberOfIndices; index++)
		localIndices[index] = modelInfo.pIndices[index];

	//---------------Pointing the model draw pointers to the local-------------------------------

	this->mModelVertexInfo.pVertices = localVertices;
	this->mModelVertexInfo.pIndices = localIndices;

	//------------------------Pushing to the node list-------------------------------------------

	this->nodesList.reserve(this->mModelVertexInfo.numberOfVertices);

	//std::cout << "Model Vertices = { ";

	for (unsigned int index = 0; index != this->mModelVertexInfo.numberOfVertices; index++)
	{
		sNode* particleNode = new sNode();

		particleNode->currentPosition.x = this->mModelVertexInfo.pVertices[index].x;
		particleNode->currentPosition.y = this->mModelVertexInfo.pVertices[index].y;
		particleNode->currentPosition.z = this->mModelVertexInfo.pVertices[index].z;

		this->mModelVertexInfo.pVertices[index] = localVertices[index];

		particleNode->oldPosition = particleNode->currentPosition;

		particleNode->modelVertexPointer = &(this->mModelVertexInfo.pVertices[index]);

		particleNode->nodeIndex = index;

		this->nodesList.push_back(particleNode);

		//std::cout << "| Vertex #" << index << " : (" << this->mModelVertexInfo.pVertices[index].x << ", " << this->mModelVertexInfo.pVertices[index].y << ", " << this->mModelVertexInfo.pVertices[index].z << ") |";
	}

	/*std::cout << " }" << std::endl;
	std::cout << std::endl;*/

	//--------------------------Creating Contraints----------------------------------------------

	for (unsigned int index = 0; index < this->mModelVertexInfo.numberOfIndices; index += 3)
	{
		sNode* node1 = this->nodesList[this->mModelVertexInfo.pIndices[index]];
		sNode* node2 = this->nodesList[this->mModelVertexInfo.pIndices[index + 1]];
		sNode* node3 = this->nodesList[this->mModelVertexInfo.pIndices[index + 2]];

		sConstraint* edge1 = new sConstraint();
		edge1->nodeA = node1;
		edge1->nodeB = node2;
		edge1->restLength = this->CalculateDistance(edge1->nodeA, edge1->nodeB);

		sConstraint* edge2 = new sConstraint();
		edge2->nodeA = node2;
		edge2->nodeB = node3;
		edge2->restLength = this->CalculateDistance(edge2->nodeA, edge2->nodeB);

		sConstraint* edge3 = new sConstraint();
		edge3->nodeA = node3;
		edge3->nodeB = node1;
		edge3->restLength = this->CalculateDistance(edge3->nodeA, edge3->nodeB);

		this->mConstraintsList.push_back(edge1);
		this->mConstraintsList.push_back(edge2);
		this->mConstraintsList.push_back(edge3);
	}

	//-------------------------Finding Center Nodes----------------------------------------------

	//FindCenterNodes();

	AddImaginaryCenterNode(softBodyPos);

	//-------------------------Check Soft Body Type---------------------------------------------

	if (this->bBallEffect)
		CreateImaginaryCenterNodeBracing();

	if (this->bChainEffect || this->bCubeEffect)
	{
		LoadTopMostNode();
		LoadBottomMostNode();
	}

	return true;
}

void cSoftBody::UpdateVertexPositions()
{
	if (this->bSoftBodyInMotion)
	{
		for (sNode* currentNode : this->nodesList)
		{
			currentNode->modelVertexPointer->x = currentNode->currentPosition.x;
			currentNode->modelVertexPointer->y = currentNode->currentPosition.y;
			currentNode->modelVertexPointer->z = currentNode->currentPosition.z;
		}
	}
}

void cSoftBody::UpdateNormals(void)
{
	for (unsigned int vertIndex = 0; vertIndex != this->mModelVertexInfo.numberOfVertices; vertIndex++)
	{
		this->mModelVertexInfo.pVertices[vertIndex].nx = 0.0f;
		this->mModelVertexInfo.pVertices[vertIndex].ny = 0.0f;
		this->mModelVertexInfo.pVertices[vertIndex].nz = 0.0f;
	}

	for (unsigned int triIndex = 0; triIndex != this->mModelVertexInfo.numberOfTriangles; triIndex++)
	{
		unsigned int vertAIndex = this->mModelVertexInfo.pIndices[triIndex + 0];
		unsigned int vertBIndex = this->mModelVertexInfo.pIndices[triIndex + 1];
		unsigned int vertCIndex = this->mModelVertexInfo.pIndices[triIndex + 2];

		sVertexWithBone& vertexA = this->mModelVertexInfo.pVertices[vertAIndex];
		sVertexWithBone& vertexB = this->mModelVertexInfo.pVertices[vertBIndex];
		sVertexWithBone& vertexC = this->mModelVertexInfo.pVertices[vertCIndex];

		glm::vec3 vertA = glm::vec3(vertexA.x, vertexA.y, vertexA.z);
		glm::vec3 vertB = glm::vec3(vertexB.x, vertexB.y, vertexB.z);
		glm::vec3 vertC = glm::vec3(vertexC.x, vertexC.y, vertexC.z);

		glm::vec3 triangleEdgeAtoB = vertB - vertA;
		glm::vec3 triangleEdgeAtoC = vertC - vertA;

		glm::vec3 theNormal = glm::cross(triangleEdgeAtoB, triangleEdgeAtoC);

		theNormal = glm::normalize(theNormal);

		vertexA.nx += theNormal.x;
		vertexA.ny += theNormal.y;
		vertexA.nz += theNormal.z;

		vertexB.nx += theNormal.x;
		vertexB.ny += theNormal.y;
		vertexB.nz += theNormal.z;

		vertexC.nx += theNormal.x;
		vertexC.ny += theNormal.y;
		vertexC.nz += theNormal.z;
	}

	for (unsigned int vertIndex = 0; vertIndex != this->mModelVertexInfo.numberOfVertices; vertIndex++)
	{
		glm::vec3 newNormal = glm::vec3(this->mModelVertexInfo.pVertices[vertIndex].nx, this->mModelVertexInfo.pVertices[vertIndex].ny, this->mModelVertexInfo.pVertices[vertIndex].nz);

		newNormal = glm::normalize(newNormal);

		this->mModelVertexInfo.pVertices[vertIndex].nx = newNormal.x;
		this->mModelVertexInfo.pVertices[vertIndex].ny = newNormal.y;
		this->mModelVertexInfo.pVertices[vertIndex].nz = newNormal.z;
	}

	return;
}

float cSoftBody::CalculateDistance(sNode* pPartA, sNode* pPartB)
{
	return glm::distance(pPartA->currentPosition, pPartB->currentPosition);
}

void cSoftBody::CalculateDampingValue(float deltaTime)
{
	if (this->bApplyDampingEffect)
	{
		float distance = glm::length(this->mImaginaryCenterNode->currentPosition - this->mImaginaryCenterNode->oldPosition);
		float minLimit = 0.001f;

		if (distance < minLimit) // Reduce damping factor as the object is closing to a full motion stop
			this->dampingFactor = this->dampingFactor - deltaTime;

		else if (distance > (minLimit * 10.f) && this->dampingFactor < 1.0f) // Re-initialize the damping factor to 1 as the object regains velocity/acceleration
		{
			this->dampingFactor = 1.f;
			bSoftBodyInMotion = true;
		}

		else if (this->dampingFactor <= minLimit) // Limit the damping factor to a particular minimum value
		{
			this->dampingFactor = 0.f;
			bSoftBodyInMotion = false;
		}
	}
}

void cSoftBody::VerletUpdate(double deltaTime)
{
	if (deltaTime > MAX_DELTATIME)
		deltaTime = MAX_DELTATIME;

	//---------------------------Calculating Damping value-------------------------------------------------

	CalculateDampingValue(deltaTime);

	//------------Calculating Verlet Integration for imaginary center node--------------------------------

	//if (bSoftBodyInMotion)
	//{
		/*glm::vec3 currentPos = this->mImaginaryCenterNode->currentPosition;
		glm::vec3 oldPos = this->mImaginaryCenterNode->oldPosition;

		glm::vec3 velocity = (currentPos - oldPos) * dampingFactor;

		this->mImaginaryCenterNode->currentPosition += velocity + (this->acceleration * static_cast<float>(deltaTime * deltaTime));

		this->mImaginaryCenterNode->oldPosition = currentPos;*/

	this->CleanZeros(this->mImaginaryCenterNode->currentPosition);
	this->CleanZeros(this->mImaginaryCenterNode->oldPosition);
	//}

	//------------------Calculating Verlet Integration for each node--------------------------------------

	if (bSoftBodyInMotion)
	{
		for (sNode* currentNode : nodesList)
		{
			glm::vec3 currentPos = currentNode->currentPosition;
			glm::vec3 oldPos = currentNode->oldPosition;

			glm::vec3 velocity = (currentPos - oldPos) * dampingFactor;

			currentNode->currentPosition += velocity + (this->acceleration * static_cast<float>(deltaTime * deltaTime));

			currentNode->oldPosition = currentPos;

			this->CleanZeros(currentNode->currentPosition);
			this->CleanZeros(currentNode->oldPosition);
		}
	}
}

void cSoftBody::ApplyCollision(double deltaTime, std::vector < cSoftBody* > softBodyCollisionList)
{
	if (this->bSoftBodyInMotion)
	{
		if (this->bBallEffect || this->bCubeEffect)
		{
			for (unsigned int nodeCount = 0; nodeCount < nodesList.size(); nodeCount++)
			{
				//------------------------Platform and walls collision----------------------------------------

				if (nodesList[nodeCount]->currentPosition.y < 0.5f)
					nodesList[nodeCount]->currentPosition.y = 0.5f;

				if (nodesList[nodeCount]->currentPosition.x > 200.0f)
					nodesList[nodeCount]->currentPosition.x = 200.0f;

				if (nodesList[nodeCount]->currentPosition.x < -200.0f)
					nodesList[nodeCount]->currentPosition.x = -200.0f;

				if (nodesList[nodeCount]->currentPosition.z > 200.0f)
					nodesList[nodeCount]->currentPosition.z = 200.0f;

				if (nodesList[nodeCount]->currentPosition.z < -200.0f)
					nodesList[nodeCount]->currentPosition.z = -200.0f;

				//----------------------------------Sphere collision-----------------------------------------

				/*glm::vec3 sphereCentre = glm::vec3(0.0f, 20.0f, 0.0f);

				float sphereRadius = 10.0f;
				float distanceToSphere = glm::distance(nodesList[nodeCount]->currentPosition, sphereCentre);

				if (distanceToSphere < sphereRadius)
				{
					glm::vec3 particleToCentreRay = nodesList[nodeCount]->currentPosition - sphereCentre;

					particleToCentreRay = glm::normalize(particleToCentreRay);

					nodesList[nodeCount]->currentPosition = (particleToCentreRay * sphereRadius) + sphereCentre;
				}*/

			}
		}

		//-----------------------Check Collision with other soft bodies-----------------------------------------

		for (cSoftBody* currentSoftBody : softBodyCollisionList)
		{
			if (currentSoftBody != this && !currentSoftBody->bChainEffect && !this->bChainEffect)
			{
				glm::vec3 currentSoftBodyPos = glm::vec3(0.f);

				float distanceToSphere = 0.f;

				for (unsigned int nodeCount = 0; nodeCount < nodesList.size(); nodeCount++)
				{
					currentSoftBodyPos = currentSoftBody->GetCurrentPos();

					distanceToSphere = glm::distance(nodesList[nodeCount]->currentPosition, currentSoftBodyPos);

					if (distanceToSphere < currentSoftBody->softBodyScaleValue)
					{
						glm::vec3 particleToCentreRay = nodesList[nodeCount]->currentPosition - currentSoftBodyPos;

						particleToCentreRay = glm::normalize(particleToCentreRay);

						nodesList[nodeCount]->currentPosition = (particleToCentreRay * currentSoftBody->softBodyScaleValue) + currentSoftBodyPos;
					}
				}
			}
		}

	}
}

void cSoftBody::SatisfyConstraints(void)
{
	if (this->bSoftBodyInMotion)
	{
		const unsigned int NUM_ITERATIONS = 10;

		for (unsigned int iteration = 0; iteration != NUM_ITERATIONS; iteration++)
		{
			for (sConstraint* currentConstraint : this->mConstraintsList)
			{
				if (currentConstraint->bIsActive)
				{
					sNode* pX1 = currentConstraint->nodeA;
					sNode* pX2 = currentConstraint->nodeB;

					glm::vec3 deltaPos = pX2->currentPosition - pX1->currentPosition;

					float deltaLength = glm::length(deltaPos);

					float diff = (deltaLength - currentConstraint->restLength) / deltaLength;

					//if (bActivateTearDown && diff > 0.9f )
					//	currentConstraint->bIsActive = false;

					// Bouncy < 1.0 to 2.0 < Breaks down
					float tightnessFactor = 1.f;

					if (!pX1->bFixedNode)
						pX1->currentPosition += deltaPos * 0.5f * diff * tightnessFactor;
					if (!pX2->bFixedNode)
						pX2->currentPosition -= deltaPos * 0.5f * diff * tightnessFactor;

					this->CleanZeros(pX1->currentPosition);
					this->CleanZeros(pX2->currentPosition);
				}
			}
		}
	}
}

void cSoftBody::CleanZeros(glm::vec3& value)
{
	const float minFloat = 1.192092896e-07f;

	if ((value.x < minFloat) && (value.x > -minFloat))
	{
		value.x = 0.0f;
	}
	if ((value.y < minFloat) && (value.y > -minFloat))
	{
		value.y = 0.0f;
	}
	if ((value.z < minFloat) && (value.z > -minFloat))
	{
		value.z = 0.0f;
	}
}

void cSoftBody::CreateRandomBracing(unsigned int numberOfBraces, float minDistanceBetweenVertices)
{
	for (unsigned int count = 0; count != numberOfBraces; count++)
	{
		bool bKeepLookingForParticles = false;

		do
		{
			bKeepLookingForParticles = false;

			unsigned int particleIndex1 = rand() % this->nodesList.size();
			unsigned int particleIndex2 = rand() % this->nodesList.size();

			sNode* node1 = this->nodesList[particleIndex1];
			sNode* node2 = this->nodesList[particleIndex2];

			float distBetween = this->CalculateDistance(node1, node2);

			if (distBetween < minDistanceBetweenVertices)
				bKeepLookingForParticles = true;
			else
			{
				sConstraint* bracingConstraint = new sConstraint();

				bracingConstraint->nodeA = node1;
				bracingConstraint->nodeB = node2;
				bracingConstraint->restLength = distBetween;

				this->mConstraintsList.push_back(bracingConstraint);
			}
		} while (bKeepLookingForParticles);
	}

	return;
}

void cSoftBody::CreateManualBracing(sNode* node1, sNode* node2)
{
	float distanceBetween = this->CalculateDistance(node1, node2);
	float offsetVal = 2.f;

	sConstraint* centerBracingConstraint = new sConstraint();

	centerBracingConstraint->nodeA = node1;
	centerBracingConstraint->nodeB = node2;

	if (centerBracingConstraint->nodeA->bFixedNode || centerBracingConstraint->nodeB->bFixedNode)
		offsetVal = 0.f;

	centerBracingConstraint->restLength = distanceBetween - offsetVal;

	this->mConstraintsList.push_back(centerBracingConstraint);
}

void cSoftBody::CreateWheelBracing()
{
	//bool continueLoopFlag = false;

	//for (unsigned int index = 0; index != this->nodesList.size(); index++)
	//{
	//
	////--------------Check if selected vertex is a center vertex-----------------
	//	
	//	sNode* softBodyVertex = this->nodesList[index];

	//	continueLoopFlag = false;

	//	for (sNode* centerVertices : centerNodesList)
	//	{
	//		if (softBodyVertex == centerVertices)
	//			continueLoopFlag = true;
	//	}

	//	if (continueLoopFlag)
	//		continue;

	////-------------Connect selected vertex with the center vertex---------------

	//	for (sNode* centerVertex : centerNodesList)
	//	{
	//		float distanceBetween = this->CalculateDistance(softBodyVertex, centerVertex);

	//		sConstraint* centerBracingConstraint = new sConstraint();

	//		centerBracingConstraint->nodeA = softBodyVertex;
	//		centerBracingConstraint->nodeB = centerVertex;
	//		centerBracingConstraint->restLength = distanceBetween;

	//		this->mConstraintsList.push_back(centerBracingConstraint);
	//	}
	//}

	return;
}

void cSoftBody::CreateImaginaryCenterNodeBracing()
{
	/*for (sNode* selectedVertex : nodesList)
	{
		float distanceBetween = this->CalculateDistance(mImaginaryCenterNode, selectedVertex);

		sConstraint* centerBracingConstraint = new sConstraint();

		centerBracingConstraint->nodeA = mImaginaryCenterNode;
		centerBracingConstraint->nodeB = selectedVertex;
		centerBracingConstraint->restLength = distanceBetween;

		this->mConstraintsList.push_back(centerBracingConstraint);
	}*/

	for (int count = 0; count < floor(nodesList.size() * 0.5); count++)
	{
		//unsigned int particleIndex2 = rand() % this->nodesList.size();

		sNode* node2 = this->nodesList[count];

		float distanceBetween = this->CalculateDistance(mImaginaryCenterNode, node2);

		sConstraint* centerBracingConstraint = new sConstraint();

		centerBracingConstraint->nodeA = mImaginaryCenterNode;
		centerBracingConstraint->nodeB = node2;
		centerBracingConstraint->restLength = distanceBetween;

		this->mConstraintsList.push_back(centerBracingConstraint);
	}
}

void cSoftBody::ApplyDirectionAcceleration(glm::vec3 directedAcceleration)
{
	/*for (int index = 0; index < centerNodesList.size(); index++)
	{
		centerNodesList[index]->currentPosition.x += directedAcceleration.x;
		centerNodesList[index]->currentPosition.y += directedAcceleration.y;
		centerNodesList[index]->currentPosition.z += directedAcceleration.z;
	}*/

	mImaginaryCenterNode->currentPosition.x += directedAcceleration.x;
	mImaginaryCenterNode->currentPosition.y += directedAcceleration.y;
	mImaginaryCenterNode->currentPosition.z += directedAcceleration.z;
}

glm::vec3 cSoftBody::GetCurrentPos()
{
	return this->mImaginaryCenterNode->currentPosition;
}

void cSoftBody::LoadTopMostNode()
{
	mTopNodesList = nodesList;

	std::sort(mTopNodesList.begin(), mTopNodesList.end(), CompareAscendingNodeYPosition);

	if (mTopNodesList.size() > 5)
		mTopNodesList.resize(10);

	for (sNode* topNode : nodesList)
		topNode->initialPosition = topNode->currentPosition;
}

void cSoftBody::LoadBottomMostNode()
{
	mBottomNodesList = nodesList;

	std::sort(mBottomNodesList.begin(), mBottomNodesList.end(), CompareDescendingNodeYPosition);

	if (mBottomNodesList.size() > 5)
		mBottomNodesList.resize(10);

	for (sNode* bottomNode : nodesList)
		bottomNode->initialPosition = bottomNode->currentPosition;
}

void cSoftBody::AddTopConnectorNode(sNode* connectingNode)
{
	this->mTopConnectorNode = connectingNode;

	CreateManualBracing(this->mTopNodesList[0], this->mTopConnectorNode);
}

void cSoftBody::AddBottomConnectorNode(sNode* connectingNode)
{
	this->mBottomConnectorNode = connectingNode;

	CreateManualBracing(this->mBottomNodesList[0], this->mBottomConnectorNode);
}

sNode* cSoftBody::GetBottomNode()
{
	return mBottomNodesList[0];
}

sNode* cSoftBody::GetTopNode()
{
	return mTopNodesList[0];
}

