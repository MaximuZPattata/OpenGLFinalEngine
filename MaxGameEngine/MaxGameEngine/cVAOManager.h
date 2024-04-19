#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

#include "pch.h"
#include "sModelDrawInfo.h"

class cVAOManager
{
public:
	bool LoadModelIntoVAO(std::string friendlyName, std::string fileName, sModelDrawInfo& drawInfo, unsigned int shaderProgramID, 
							bool bIsDynamicBuffer = false);

	bool LoadAnimationIntoModel(sModelDrawInfo& modelInfo, std::string fileName, std::string animationName);

	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo);

	void setBaseModelPath(std::string basePathWithoutSlash);
	void setBaseAnimationPath(std::string basePathWithoutSlash);

	std::string getBaseModelPath();

	bool UpdateVAOBuffers(std::string fileName, sModelDrawInfo& updatedDrawInfo, unsigned int shaderProgramID);

	// Only updates the vertex buffer information
	bool UpdateVertexBuffers(std::string fileName, sModelDrawInfo& updatedDrawInfo, unsigned int shaderProgramID);

private:

	bool m_LoadTheFile(std::string fileName, sModelDrawInfo& drawInfo);

	std::map< std::string, sModelDrawInfo> m_map_ModelName_to_VAOID;

	std::string mBaseModelPath;

	std::string mBaseAnimationPath;
};

#endif
