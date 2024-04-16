#pragma once

#include "pch.h"
#include "cMesh.h"
#include "cFBO.h"
#include "TextureManager/cBasicTextureManager.h"
#include "sModelDrawInfo.h"
#include "cVAOManager.h"
#include "cCubeMap.h"
#include "cLightManager.h"
#include "cDebugRender.h"

class cSceneManager
{
public:

	cSceneManager() {};
	~cSceneManager() {};

	struct sSceneDetails
	{
		struct sSceneCameraAngles
		{
			unsigned int cameraAngleId = 0;

			glm::vec3 sceneCameraPosition = glm::vec3(0.f);
			glm::vec3 sceneCameraTarget = glm::vec3(0.f);
		};

		unsigned int sceneID = 0;
		unsigned int cameraAngleIDCounter = 0;

		bool bTurnIntoFBOTexture = false;
		bool bSceneHasTransparencyMesh = false;
		bool bMainScene = false; // This is the scene viewed in the world view and not as a texture

		glm::vec3 sceneCameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

		std::vector < cFBO* > FBOList;
		std::vector < sSceneCameraAngles* > cameraAnglesList;
		std::vector < cMesh* > sceneMeshList;
		std::vector < cMesh* > sceneDebugMeshList;
	};

	void InitializeSceneManager(cVAOManager* newVAOManager, cBasicTextureManager* newTextureManager, cCubeMap* newCubeManager, cLightManager* newLightManager);
	void LoadScene(GLuint shaderProgramID, GLFWwindow* window, glm::vec3 mainCameraPosition, glm::vec3 mainCameraTarget, bool isFreeFlowCam);
	void SortMeshesBasedOnTransparency(glm::vec3 currentCamPos, glm::vec3 currentCamTarget, std::vector < cMesh* > currentSceneMeshList);
	void CreateScene(bool isMainScene, std::vector <glm::vec3> camPos, std::vector <glm::vec3> camTarget);
	void AddMeshToScene(unsigned int sceneId, cMesh* sceneMesh);
	void AddDebugMeshToScene(unsigned int sceneId, cMesh* debugMesh, cDebugRenderer* debugRenderManager);
	void ConvertSceneToFBOTexture(unsigned int sceneId, unsigned int windowWidth, unsigned int windowHeight);
	void CheckTransparency(unsigned int meshSceneId);
	void DrawSceneObject(cMesh* currentMesh, glm::mat4 matModelParent, GLuint shaderProgramID, int screenWidth, int screenHeight, bool doNotApplyMatrixTransformation = false);
	void DrawSkyBox(cMesh* currentMesh, GLuint shaderProgramID, glm::vec3(cameraEye), int screenWidth, int screenHeight);
	void AddFBOTextureToMesh(unsigned int sceneIdOfFBOTexture, cMesh* meshNeedingTexture, unsigned int fboID);
	void AddSecondPassFilter(cMesh* meshTextureNeedingFilter, bool useFSQ, bool useStandardColor, bool useChromicAberration, bool UseScreenMeasurements, bool useNightVision);

private:
	unsigned int sceneIdCounter = 0;

	cVAOManager* mSceneVAOManager = NULL;
	cBasicTextureManager* mSceneTextureManager = NULL;
	cCubeMap* mSceneCubeMapManager = NULL;
	cDebugRenderer* mDebugRenderManager = NULL;
	cLightManager* mLightManager = NULL;

	std::vector < sSceneDetails* > mSceneDetailsList;

	void CalculateBoneMatrices(cMesh* pCurrentMesh, Node* node, const glm::mat4& parentTransformationMatrix, sModelDrawInfo* modelInfo);
};

