#ifndef _cMesh_HG_
#define _cMesh_HG_

#include "pch.h"
#include "sModelDrawInfo.h"

struct sMeshTexture
{
	bool bUseHeightMap = false;
	bool bDiscardBlackAreasInHeightMap = false;
	bool bDiscardColoredAreasInHeightMap = false;
	bool bUseDiscardMaskTexture = false;

	float heightMapScale = 0.0f;

	int textureUnit;
	int discardMaskTextureUnit;

	std::string texturePath;
	std::string textureName;
	std::string discardMaskTexturePath;

	glm::vec2 UVOffset = glm::vec2(0.0f, 0.0f);
};

struct sBlendedTextures
{
	std::vector <std::string> texturePathList;
	std::vector <std::string> textureNameList;
	float textureRatiosList[4];
	std::vector <int> textureUnit;
};

struct sSecondPassFilter
{
	float chromicAberrationAmount = 0.5f;
	bool reverseAnimation = false;

	bool bUseChromicAberration = false;
	bool bUseStandardColorTexture = true;
	bool bUseTextureWithScreenWidthAndHeight = false;
	bool bUseFSQ = false;
	bool bUseNightVision = false;
};

class cMesh 
{
private:	
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;

	glm::quat m_qOrientation;

public:
	cMesh();
	~cMesh();

	float alphaTransparency;

	unsigned int FBOTextureNumber;
	unsigned int sceneId = 0;

	bool bIsVisible;
	bool bIsWireframe;
	bool bDoNotLight;
	bool bUseManualColours;
	bool bUseTextures;
	bool bIsSkyBox;
	bool bIsAABBMesh;
	bool bIsSoftBody;
	bool bIsDebugMesh;
	bool bTextureIsFromFBO;
	bool bMeshUsesSecondPassFilter;
	bool bApplyBones = false;
	bool bDisplayBoneWeightColor = false;

	std::string meshName;
	std::string friendlyName;

	glm::vec3 drawPosition;
	glm::vec3 drawOrientation;
	glm::vec3 drawScale;

	glm::vec4 wholeObjectManualColourRGBA;

	std::vector<cMesh*> mChildMeshesList;
	std::vector <sMeshTexture> mTextureDetailsList;

	std::map<std::string, glm::mat4> mBoneTransformationsMap;

	sBlendedTextures mMixedTextures;

	sSecondPassFilter mSecondPassFilters;

	sModelDrawInfo* modelDrawInfo;

	unsigned int getUniqueID(void);

	void setUniformDrawScale(float scale);

	void setDrawPosition(const glm::vec3& newPosition);
	void setDrawOrientation(const glm::quat& newOrientation, glm::vec3 newAngles);
	void setDrawOrientationQuaternion(const glm::quat& newOrientation);

	glm::vec3 getDrawPosition(void);
	glm::vec3 getDrawOrientation(void);

	glm::quat get_qOrientation(void)
	{
		return this->m_qOrientation;
	}
};

#endif

