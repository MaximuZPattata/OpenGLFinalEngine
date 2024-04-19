#include "pch.h"
#include "cSceneManager.h"

#include <algorithm>
#include <thread>

void cSceneManager::InitializeSceneManager(cVAOManager* newVAOManager, cBasicTextureManager* newTextureManager, cCubeMap* newCubeManager, cLightManager* newLightManager)
{
    this->mSceneVAOManager = newVAOManager;
    this->mSceneTextureManager = newTextureManager;
    this->mSceneCubeMapManager = newCubeManager;
    this->mLightManager = newLightManager;
}

void cSceneManager::LoadScene(GLuint shaderProgramID, GLFWwindow* window, std::vector <cMesh*> &MeshList, glm::vec3 mainCameraPosition, glm::vec3 mainCameraTarget, bool isFreeFlowCam, bool applyLOD)
{
    glUseProgram(shaderProgramID);

    for (sSceneDetails* currentScene : this->mSceneDetailsList)
    {
        //----------------------------Initialize Scene Details------------------------------------

        float ratio;
        int width, height;

        glm::vec3 currentCamEye;
        glm::vec3 currentCamTarget;

        //-----------------Check how many camera angles of the scene needed-----------------------

        for (int camAngleCount = 0; camAngleCount < currentScene->cameraAnglesList.size(); camAngleCount++)
        {
            sSceneDetails::sSceneCameraAngles* currentCameraAngle = currentScene->cameraAnglesList[camAngleCount];

            //-----------------------Check if scene is rendered offScreen-----------------------------

            if (currentScene->bTurnIntoFBOTexture)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, currentScene->FBOList[camAngleCount]->ID);

                width = currentScene->FBOList[camAngleCount]->width;
                height = currentScene->FBOList[camAngleCount]->height;

                currentScene->FBOList[camAngleCount]->clearBuffers(true, true);
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glfwGetFramebufferSize(window, &width, &height);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            ratio = width / static_cast<float>(height);

            glViewport(0, 0, width, height);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            //------------------------------Load Camera Values----------------------------------------

            if (currentScene->bMainScene)
            {
                currentCameraAngle->sceneCameraPosition = mainCameraPosition;
                currentCameraAngle->sceneCameraTarget = mainCameraTarget;
            }

            currentCamEye = currentCameraAngle->sceneCameraPosition;
            currentCamTarget = currentCameraAngle->sceneCameraTarget;

            GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");

            glUniform4f(eyeLocation_UL, currentCamEye.x, currentCamEye.y, currentCamEye.z, 1.0f);

            glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 10'000.0f);

            glm::mat4 matView;

            if (currentScene->bTurnIntoFBOTexture)
                matView = glm::lookAt(currentCamEye, currentCamTarget, currentScene->sceneCameraUpVector);
            else
            {
                if(isFreeFlowCam)
                    matView = glm::lookAt(currentCamEye, currentCamEye + currentCamTarget, currentScene->sceneCameraUpVector);
                else
                    matView = glm::lookAt(currentCamEye, currentCamTarget, currentScene->sceneCameraUpVector);
            }

            GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
            glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

            GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
            glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

            //------------------------------Draw Scene objects--------------------------------------------

            //this->ProcessMeshesInParallel(shaderProgramID, currentCamEye, currentCamTarget, , currentScene, width, height, applyLOD);

            ProcessMeshesInParallel(MeshList, currentScene->sceneID, currentCamEye, currentCamTarget, applyLOD, shaderProgramID, width, height, currentScene);
            
            //-----------------------------Draw Debug Objects---------------------------------------------

            for (int index = 0; index < currentScene->sceneDebugMeshList.size(); index++)
            {
                std::vector<glm::vec3> spherePositions = mDebugRenderManager->GetSphereModelPositions(currentScene->sceneDebugMeshList[index]->meshUniqueName);

                for (int count = 0; count < spherePositions.size(); count++)
                {
                    currentScene->sceneDebugMeshList[index]->bIsVisible = true;
                    currentScene->sceneDebugMeshList[index]->drawPosition = spherePositions[count];

                    this->DrawSceneObject(currentScene->sceneDebugMeshList[index], glm::mat4(1.0f), shaderProgramID, width, height);
                }
            }
        }
    }
}

DWORD WINAPI ProcessMeshList(LPVOID lpParameter)
{
    auto params = static_cast<std::tuple<std::vector<cMesh*>&, int, int, int, glm::vec3&, glm::vec3&, bool, GLuint, int, int, cSceneManager::sSceneDetails*, cSceneManager*>*>(lpParameter);

    auto& MeshList = std::get<0>(*params);
    int startIndex = std::get<1>(*params);
    int endIndex = std::get<2>(*params);
    int sceneID = std::get<3>(*params);
    glm::vec3& currentCamEye = std::get<4>(*params);
    glm::vec3& currentCamTarget = std::get<5>(*params);
    bool applyLOD = std::get<6>(*params);
    GLuint shaderProgramID = std::get<7>(*params);
    int width = std::get<8>(*params);
    int height = std::get<9>(*params);
    cSceneManager::sSceneDetails* currentScene = std::get<10>(*params);
    cSceneManager* sceneManager = std::get<11>(*params);

    cMesh* skyBox = nullptr;

    for (int index = startIndex; index < endIndex; ++index)
    {
        cMesh* currentMesh = MeshList[index];

        if (currentMesh->sceneId == sceneID)
        {
            // Check for LOD
            if (applyLOD && currentMesh->bUseLOD && !currentMesh->CheckForLOD(currentCamEye))
                continue;

            // Check for SkyBox
            if (currentMesh->bIsSkyBox && !currentScene->bSceneHasTransparencyMesh)
                skyBox = currentMesh;
            else if (currentMesh->bIsVisible)
            {
                if (!currentMesh->bIsSkyBox)
                {
                    glm::mat4 matModel = glm::mat4(1.0f);
                    sceneManager->DrawSceneObject(currentMesh, matModel, shaderProgramID, width, height, currentMesh->bIsSoftBody);
                }
                else
                    sceneManager->DrawSkyBox(currentMesh, shaderProgramID, currentCamEye, width, height);
            }
        }
    }

    return 0;
}

void cSceneManager::ProcessMeshesInParallel(std::vector<cMesh*>& MeshList, int sceneID, glm::vec3& currentCamEye, glm::vec3& currentCamTarget, bool applyLOD, 
    GLuint shaderProgramID, int width, int height, sSceneDetails* currentScene)
{
    int numMeshes = static_cast<int>(MeshList.size());
    int halfNumMeshes = numMeshes / 2;

    auto params1 = std::make_tuple(std::ref(MeshList), 0, halfNumMeshes, sceneID, std::ref(currentCamEye), std::ref(currentCamTarget),
        applyLOD, shaderProgramID, width, height, currentScene, this);

    auto params2 = std::make_tuple(std::ref(MeshList), halfNumMeshes, numMeshes, sceneID, std::ref(currentCamEye), std::ref(currentCamTarget),
        applyLOD, shaderProgramID, width, height, currentScene, this);

    HANDLE thread1 = CreateThread(NULL, 0, ProcessMeshList, &params1, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, ProcessMeshList, &params2, 0, NULL);

    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    CloseHandle(thread1);
    CloseHandle(thread2);
}

//void cSceneManager::ProcessMeshDrawList(GLuint shaderProgramID, glm::vec3& currentCamEye, glm::vec3& currentCamTarget, std::vector<cMesh*>& MeshList, sSceneDetails* currentScene, int startIndex, int endIndex, float width, float height, bool applyLOD)
//{
//    cMesh* skyBox = NULL;
//
//    // Sorts the TotalMeshList based on distance between camera position and mesh object positions 
//    if (currentScene->bSceneHasTransparencyMesh)
//        SortMeshesBasedOnTransparency(currentScene->sceneID, currentCamEye, currentCamTarget, MeshList);
//
//    for (unsigned int index = startIndex; index != endIndex; index++)
//    {
//        if (MeshList[index]->sceneId == currentScene->sceneID)
//        {
//            cMesh* currentMesh = MeshList[index];
//
//            // Check for LOD
//            if (applyLOD)
//            {
//                if (currentMesh->bUseLOD)
//                {
//                    if (!currentMesh->CheckForLOD(currentCamEye))
//                        continue;
//                }
//            }
//
//            // Check for SkyBox
//            if (currentMesh->bIsSkyBox && !currentScene->bSceneHasTransparencyMesh)
//                skyBox = currentMesh;
//            else
//            {
//                if (currentMesh->bIsVisible)
//                {
//                    if (!currentMesh->bIsSkyBox)
//                    {
//                        glm::mat4 matModel = glm::mat4(1.0f);
//
//                        this->DrawSceneObject(currentMesh, matModel, shaderProgramID, width, height, currentMesh->bIsSoftBody);
//                    }
//                    else
//                        this->DrawSkyBox(currentMesh, shaderProgramID, currentCamEye, width, height);
//                }
//            }
//
//            // This is to draw skyBox at the last if there are no transparent objects in the 3D world
//            if (index == MeshList.size() - 1 && skyBox != NULL)
//                this->DrawSkyBox(skyBox, shaderProgramID, currentCamEye, width, height);
//        }
//    }
//}
//
////Function to divide the mesh list and process it using threads
//void cSceneManager::ProcessMeshesInParallel(GLuint shaderProgramID, glm::vec3& currentCamEye, glm::vec3& currentCamTarget, std::vector<cMesh*>& MeshList, sSceneDetails* currentScene, float width, float height, bool applyLOD)
//{
//    int numMeshes = static_cast<int>(MeshList.size());
//    int halfNumMeshes = numMeshes / 2;
//
//    std::thread thread1(ProcessMeshDrawList, std::ref(currentCamEye), std::ref(currentCamTarget), std::ref(MeshList), currentScene, 
//        0, halfNumMeshes, width, height, applyLOD);
//   
//    std::thread thread2(ProcessMeshDrawList, std::ref(currentCamEye), std::ref(currentCamTarget), std::ref(MeshList), currentScene, 
//        halfNumMeshes, numMeshes, width, height, applyLOD);
// 
//    thread1.join();
//    thread2.join();
//}

void cSceneManager::SortMeshesBasedOnTransparency(unsigned int currentSceneId, glm::vec3 currentCamPos, glm::vec3 currentCamTarget, std::vector < cMesh* >& MeshList)
{
    glm::vec3 cameraPos = currentCamPos;
    glm::vec3 cameraPerspective = currentCamTarget;
    glm::vec3 cameraDirection = glm::normalize(cameraPerspective - cameraPos);

    float offsetVal = 5'000.0f;

    // Making position of skyBox 5'000.0f away from the view of the camera
    glm::vec3 skyBoxPos = cameraPos + offsetVal * cameraDirection;

    std::sort(MeshList.begin(), MeshList.end(), [cameraPos, skyBoxPos](cMesh* mesh1, cMesh* mesh2)
        {
                if (mesh1->bIsSkyBox)
                    return (glm::distance(skyBoxPos, cameraPos) > glm::distance(mesh2->drawPosition, cameraPos));
                else if (mesh2->bIsSkyBox)
                    return (glm::distance(mesh1->drawPosition, cameraPos) > glm::distance(skyBoxPos, cameraPos));
                else
                    return (glm::distance(mesh1->drawPosition, cameraPos) > glm::distance(mesh2->drawPosition, cameraPos));
        });
}

void cSceneManager::CalculateBoneMatrices(cMesh* boneMesh, Node* node, const glm::mat4& parentTransformationMatrix, sModelDrawInfo* modelInfo)
{
    glm::mat4 nodeTransform = node->Transformation;

    //--------------------To check if node is already added to the 'Bone Transformation' Map--------------------------

    std::map<std::string, glm::mat4>::iterator boneIt = boneMesh->mBoneTransformationsMap.find(node->Name);

    if (boneIt != boneMesh->mBoneTransformationsMap.end())
        nodeTransform = boneIt->second;

    //------------------------------------Convert to global transform-------------------------------------------------

    glm::mat4 globalTransformation = parentTransformationMatrix * nodeTransform;

    //------------------------To check if node has underwent world matrix transformation------------------------------

    auto boneMapIt = modelInfo->BoneNameToIdMap.find(node->Name);

    if (boneMapIt != modelInfo->BoneNameToIdMap.end())
    {
        BoneInfo& boneInfo = modelInfo->BoneInfoList[boneMapIt->second];

        boneInfo.WorldTransformation = modelInfo->GlobalInverseTransformation * globalTransformation * boneInfo.BoneOffset;
    }

    //---------------------Calculating world transformations for the node's children if any---------------------------

    for (int i = 0; i < node->Children.size(); ++i)
        CalculateBoneMatrices(boneMesh, node->Children[i], globalTransformation, modelInfo);
}

void cSceneManager::DrawSceneObject(cMesh* currentMesh, glm::mat4 matModelParent, GLuint shaderProgramID, int screenWidth, int screenHeight, bool doNotApplyMatrixTransformation)
{
    //--------------------------Calculate Matrix Model Transformation--------------------------------

    glm::mat4 matModel = matModelParent;

    glm::mat4 matTranslate = glm::mat4(1.0f);
    glm::mat4 matRotation = glm::mat4(1.0f);
    glm::mat4 matScale = glm::mat4(1.0f);

    if (!doNotApplyMatrixTransformation)
    {
        matTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(currentMesh->drawPosition.x, currentMesh->drawPosition.y, currentMesh->drawPosition.z));
        matRotation = glm::mat4_cast(currentMesh->get_qOrientation());
        matScale = glm::scale(glm::mat4(1.0f), glm::vec3(currentMesh->drawScale.x, currentMesh->drawScale.y, currentMesh->drawScale.z));
    }

    matModel = matModel * matTranslate;

    matModel = matModel * matRotation;

    matModel = matModel * matScale;

    //-------------------------Get Model Info--------------------------------------------------------

    GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));

    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));

    // ---------------------Check Light and Wireframe-------------------------------------------------

    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (currentMesh->bIsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (currentMesh->bDoNotLight)
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    else
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);

    //-------------------------Get Manual Color from Shader----------------------------------------

    GLint bUseManualColour_UL = glGetUniformLocation(shaderProgramID, "bUseManualColour");

    if (currentMesh->bUseManualColours)
    {
        glUniform1f(bUseManualColour_UL, (GLfloat)GL_TRUE);

        GLint manualColourRGBA_UL = glGetUniformLocation(shaderProgramID, "manualColourRGBA");
        glUniform4f(manualColourRGBA_UL,
            currentMesh->wholeObjectManualColourRGBA.r,
            currentMesh->wholeObjectManualColourRGBA.g,
            currentMesh->wholeObjectManualColourRGBA.b,
            currentMesh->wholeObjectManualColourRGBA.a);
    }
    else
        glUniform1f(bUseManualColour_UL, (GLfloat)GL_FALSE);

    //------------------------------Add Textures----------------------------------------------

    GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramID, "bUseDiscardMaskTexture");
    GLint bUseTextureColour_UL = glGetUniformLocation(shaderProgramID, "bUseTextureColour");
    GLint textureFBO_UL = glGetUniformLocation(shaderProgramID, "bUseFBO");
    GLint textureFSQ_UL = glGetUniformLocation(shaderProgramID, "bUseFSQ");
    GLint textureScreenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "bUseScreenWidthAndHeightForTexture");
    GLint textureChromicAberration_UL = glGetUniformLocation(shaderProgramID, "bUseChromicAberration");
    GLint textureStandard_UL = glGetUniformLocation(shaderProgramID, "bUseStandardColor");
    GLint textureNightVision_UL = glGetUniformLocation(shaderProgramID, "bUseNightVisionTexture");

    // Making all texture related boolean to false
    glUniform1f(bUseTextureColour_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureFBO_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureFSQ_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureScreenWidthAndHeight_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureChromicAberration_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureStandard_UL, (GLfloat)GL_FALSE);
    glUniform1f(textureNightVision_UL, (GLfloat)GL_FALSE);

    if (currentMesh->bUseTextures)
    {
        GLint bUseHeightMap_UL = glGetUniformLocation(shaderProgramID, "bUseHeightMap");
        GLint textureMix_UL = glGetUniformLocation(shaderProgramID, "bUseTextureMixRatio");
        GLint bDiscardBlackAreas_UL = glGetUniformLocation(shaderProgramID, "bDiscardBlackInHeightMap");
        GLint bDiscardColoredAreas_UL = glGetUniformLocation(shaderProgramID, "bDiscardColoredAreasInHeightMap");

        glUniform1f(bDiscardBlackAreas_UL, (GLfloat)GL_FALSE);
        glUniform1f(bDiscardColoredAreas_UL, (GLfloat)GL_FALSE);
        glUniform1f(textureMix_UL, (GLfloat)GL_FALSE);
        glUniform1f(bUseHeightMap_UL, (GLfloat)GL_FALSE);
        glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_FALSE);

        glUniform1f(bUseTextureColour_UL, (GLfloat)GL_TRUE);

        for (int textureIndex = 0; textureIndex < currentMesh->mTextureDetailsList.size(); textureIndex++)
        {
            //-----------------------------To apply Height Map to the texture--------------------------------------------

            if (currentMesh->mTextureDetailsList[textureIndex].bUseHeightMap)
            {
                //-----------------------Add Height Map texture Sampler---------------------------------------------

                GLuint textureToBeAdded = this->mSceneTextureManager->getTextureIDFromName(currentMesh->mTextureDetailsList[textureIndex].texturePath);

                glActiveTexture(GL_TEXTURE0 + currentMesh->mTextureDetailsList[textureIndex].textureUnit);

                glBindTexture(GL_TEXTURE_2D, textureToBeAdded);

                GLint texture_HM_UL = glGetUniformLocation(shaderProgramID, "heightMapSampler");
                glUniform1i(texture_HM_UL, currentMesh->mTextureDetailsList[textureIndex].textureUnit);

                //--------------------Adjust scale and UVOffset of Height Map--------------------------------------

                glUniform1f(bUseHeightMap_UL, (GLfloat)GL_TRUE);

                GLint heightScale_UL = glGetUniformLocation(shaderProgramID, "heightScale");
                GLint UVOffset_UL = glGetUniformLocation(shaderProgramID, "UVOffset");

                glUniform1f(heightScale_UL, currentMesh->mTextureDetailsList[textureIndex].heightMapScale);

                glUniform2f(UVOffset_UL, currentMesh->mTextureDetailsList[textureIndex].UVOffset.x, currentMesh->mTextureDetailsList[textureIndex].UVOffset.y);

                //----------------Discard colored or uncolored areas of Height Map---------------------------------

                if (currentMesh->mTextureDetailsList[textureIndex].bDiscardBlackAreasInHeightMap)
                    glUniform1f(bDiscardBlackAreas_UL, (GLfloat)GL_TRUE);

                if (currentMesh->mTextureDetailsList[textureIndex].bDiscardColoredAreasInHeightMap)
                    glUniform1f(bDiscardColoredAreas_UL, (GLfloat)GL_TRUE);
            }

            else
            {
                //--------------------------------Apply normal texture-------------------------------------------------------

                GLuint textureToBeAdded = this->mSceneTextureManager->getTextureIDFromName(currentMesh->mTextureDetailsList[textureIndex].texturePath);

                glActiveTexture(GL_TEXTURE0 + currentMesh->mTextureDetailsList[textureIndex].textureUnit);

                glBindTexture(GL_TEXTURE_2D, textureToBeAdded);

                GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "textureAdder");
                glUniform1i(texture_00_UL, currentMesh->mTextureDetailsList[textureIndex].textureUnit);

                //--------------------------------To apply Discard Mask Texture-----------------------------------------------

                if (currentMesh->mTextureDetailsList[textureIndex].bUseDiscardMaskTexture)
                {
                    glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_TRUE);

                    GLuint textureToBeMasked = this->mSceneTextureManager->getTextureIDFromName(currentMesh->mTextureDetailsList[textureIndex].discardMaskTexturePath);

                    glActiveTexture(GL_TEXTURE0 + currentMesh->mTextureDetailsList[textureIndex].discardMaskTextureUnit);

                    glBindTexture(GL_TEXTURE_2D, textureToBeMasked);

                    GLint maskSampler_UL = glGetUniformLocation(shaderProgramID, "maskSampler");
                    glUniform1i(maskSampler_UL, currentMesh->mTextureDetailsList[textureIndex].discardMaskTextureUnit);
                }
            }
        }

        //--------------------------------To apply Blended Textures-----------------------------------------------------

        if (currentMesh->mMixedTextures.texturePathList.size() > 0)
        {
            GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");

            glUniform1f(textureMix_UL, (GLfloat)GL_TRUE);

            for (int texturesCount = 0; texturesCount < currentMesh->mMixedTextures.texturePathList.size(); texturesCount++)
            {
                GLuint textureToBeAdded = this->mSceneTextureManager->getTextureIDFromName(currentMesh->mMixedTextures.texturePathList[texturesCount]);

                glActiveTexture(GL_TEXTURE0 + currentMesh->mMixedTextures.textureUnit[texturesCount]);

                glBindTexture(GL_TEXTURE_2D, textureToBeAdded);

                std::string textureSamplerName = "texture_0" + std::to_string(texturesCount);

                GLint texture_00_UL = glGetUniformLocation(shaderProgramID, textureSamplerName.c_str());
                glUniform1i(texture_00_UL, currentMesh->mMixedTextures.textureUnit[texturesCount]);
            }

            // Adding texture ratios
            glUniform4f(textureMixRatio_0_3_UL,
                currentMesh->mMixedTextures.textureRatiosList[0],
                currentMesh->mMixedTextures.textureRatiosList[1],
                currentMesh->mMixedTextures.textureRatiosList[2],
                currentMesh->mMixedTextures.textureRatiosList[3]);
        }
    }

    //---------------------------------To apply FBO as Texture------------------------------------------------------

    else if (currentMesh->bTextureIsFromFBO)
    {
        GLint textureUnitNumber = 0;
        GLint texture_FBO = glGetUniformLocation(shaderProgramID, "textureFBO");
        GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");

        glUniform1f(textureFBO_UL, (GLfloat)GL_TRUE);

        //-------------------------Fetching texture values-------------------------------------------

        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, currentMesh->FBOTextureNumber);
        glUniform1i(texture_FBO, textureUnitNumber);

        GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");

        glUniform2f(screenWidthAndHeight_UL, (GLfloat)screenWidth, (GLfloat)screenHeight);

        //------------------------Adding Second Pass Filters-----------------------------------------

        if (currentMesh->mSecondPassFilters.bUseChromicAberration)
        {
            glUniform1f(textureChromicAberration_UL, (GLfloat)GL_TRUE);

            GLint chromicAmount_UL = glGetUniformLocation(shaderProgramID, "chromicAmount");
            glUniform1f(chromicAmount_UL, currentMesh->mSecondPassFilters.chromicAberrationAmount);
        }

        if (currentMesh->mSecondPassFilters.bUseFSQ)
            glUniform1f(textureFSQ_UL, (GLfloat)GL_TRUE);

        if (currentMesh->mSecondPassFilters.bUseTextureWithScreenWidthAndHeight)
            glUniform1f(textureScreenWidthAndHeight_UL, (GLfloat)GL_TRUE);

        if (currentMesh->mSecondPassFilters.bUseStandardColorTexture)
            glUniform1f(textureStandard_UL, (GLfloat)GL_TRUE);

        if (currentMesh->mSecondPassFilters.bUseNightVision)
            glUniform1f(textureNightVision_UL, (GLfloat)GL_TRUE);
    }

    //--------------------------------Add Bones-----------------------------------------------

    GLint bUseBone_UL = glGetUniformLocation(shaderProgramID, "bUseBones");
    GLint bUseBoneWeightColor_UL = glGetUniformLocation(shaderProgramID, "bUseBoneWeightColor");

    if (currentMesh->bApplyBones)
    {
        glUniform1f(bUseBone_UL, (GLfloat)GL_TRUE);

        sModelDrawInfo* modelBoneDrawInfo = currentMesh->modelDrawInfo;

        CalculateBoneMatrices(currentMesh, modelBoneDrawInfo->RootNode, glm::mat4(1.0f), modelBoneDrawInfo);

        for (int j = 0; j < modelBoneDrawInfo->BoneInfoList.size(); ++j)
        {
            glm::mat4 boneMatrix = modelBoneDrawInfo->BoneInfoList[j].WorldTransformation;
            std::string boneUL = "BoneMatrices[" + std::to_string(j) + "]";

            GLint boneUL_ID = glGetUniformLocation(shaderProgramID, boneUL.c_str());
            glUniformMatrix4fv(boneUL_ID, 1, GL_FALSE, glm::value_ptr(boneMatrix));
        }

        if (currentMesh->bDisplayBoneWeightColor)
            glUniform1f(bUseBoneWeightColor_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bUseBone_UL, (GLfloat)GL_FALSE);
        glUniform1f(bUseBoneWeightColor_UL, (GLfloat)GL_FALSE);
    }

    //-----------------------------------Setup Cube Map----------------------------------------------

    if (this->mSceneCubeMapManager != NULL)
    {
        GLuint skyBoxID = this->mSceneTextureManager->getTextureIDFromName(this->mSceneCubeMapManager->GetSkyBoxName());

        glActiveTexture(GL_TEXTURE0 + this->mSceneCubeMapManager->GetSkyBoxTextureUnit());

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);

        GLint cubeMapSampler_UL = glGetUniformLocation(shaderProgramID, "cubeMapSampler");
        glUniform1i(cubeMapSampler_UL, this->mSceneCubeMapManager->GetSkyBoxTextureUnit());
    }

    //-------------------------Check Alpha transparency----------------------------------------

    if (currentMesh->alphaTransparency < 1.0f) // Transparent
    {
        glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_FALSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else // Opaque
        glDisable(GL_BLEND);

    GLint aplhaTransparency_UL = glGetUniformLocation(shaderProgramID, "alphaTransparency");
    glUniform1f(aplhaTransparency_UL, currentMesh->alphaTransparency);

    //--------------------------------Draw Model-----------------------------------------------

    sModelDrawInfo& modelInfo = *currentMesh->modelDrawInfo;

    glBindVertexArray(modelInfo.VAO_ID);
    glDrawElements(GL_TRIANGLES, modelInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //------------------------------Remove Scaling---------------------------------------------

    glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(1.0f / currentMesh->drawScale.x, 1.0f / currentMesh->drawScale.y, 1.0f / currentMesh->drawScale.z));

    matModel = matModel * matRemoveScaling;

    return;
}

void cSceneManager::DrawSkyBox(cMesh* skyBox, GLuint shaderProgramID, glm::vec3(cameraEye), int width, int height)
{
    //------------------------------Make DiscardMask false---------------------------------------

    GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramID, "bUseDiscardMaskTexture");

    glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_FALSE);

    //------------------------------Draw SkyBox last----------------------------------------------

    //ScaleModel(cubeMapManager->GetMeshModelName(), 10.0f);
    //MoveModel(cubeMapManager->GetMeshModelName(), 0.0f, 100.0f, 0.0f);

    skyBox->setUniformDrawScale(5'000.0f);
    skyBox->setDrawPosition(cameraEye);

    GLint bApplySkyBox_UL = glGetUniformLocation(shaderProgramID, "bApplySkyBox");
    glUniform1f(bApplySkyBox_UL, (GLfloat)GL_TRUE);

    // Making mesh normals to face inward instead of outward
    glCullFace(GL_FRONT);

    this->DrawSceneObject(skyBox, glm::mat4(1.0f), shaderProgramID, width, height);

    glUniform1f(bApplySkyBox_UL, (GLfloat)GL_FALSE);

    // Making normals face outward
    glCullFace(GL_BACK);
}

void cSceneManager::CheckTransparency(unsigned int meshSceneId, std::vector < cMesh* >& MeshList)
{
    bool transparencyFlag = false;

    for (int index = 0; index < MeshList.size(); index++)
    {
        cMesh* currentMesh = MeshList[index];

        if (currentMesh->alphaTransparency < 1.0f)
        {
            transparencyFlag = true;
            break;
        }
    }

    this->mSceneDetailsList[meshSceneId]->bSceneHasTransparencyMesh = transparencyFlag;
}

void cSceneManager::CreateScene(bool isMainScene, std::vector <glm::vec3> camPos, std::vector <glm::vec3> camTarget)
{
    sSceneDetails* newScene = new sSceneDetails();

    newScene->sceneID = this->sceneIdCounter;

    for (int cameraAngleCount = 0; cameraAngleCount < camPos.size(); cameraAngleCount++)
    {
        sSceneDetails::sSceneCameraAngles* newCameraAngle = new sSceneDetails::sSceneCameraAngles();

        newCameraAngle->cameraAngleId = newScene->cameraAngleIDCounter;
        newCameraAngle->sceneCameraPosition = camPos[cameraAngleCount];
        newCameraAngle->sceneCameraTarget = camTarget[cameraAngleCount];

        newScene->cameraAngleIDCounter++;
        newScene->cameraAnglesList.push_back(newCameraAngle);
    }

    newScene->bMainScene = isMainScene;

    this->sceneIdCounter++;

    this->mSceneDetailsList.push_back(newScene);

    std::cout << "SCENE SUCCESFULLY LOADED | SCENE ID : " << newScene->sceneID << std::endl;
}

void cSceneManager::ConvertSceneToFBOTexture(unsigned int sceneId, unsigned int windowWidth, unsigned int windowHeight)
{
    if (sceneId < 0 || sceneId >= this->mSceneDetailsList.size())
    {
        std::cout << "ERROR : COULDN'T CONVERT SCENE TO FBO | SCENE ID PROVIDED DOES NOT MATCH" << std::endl;

        return;
    }

    sSceneDetails* selectedScene = this->mSceneDetailsList[sceneId];

    std::string FBOError;

    for (int FBOIndex = 0; FBOIndex < selectedScene->cameraAnglesList.size(); FBOIndex++)
    {
        cFBO* newFBO = new cFBO();

        selectedScene->bTurnIntoFBOTexture = true;

        if (newFBO->init(windowWidth, windowHeight, FBOError))
        {
            std::cout << std::endl;
            std::cout << "SUCCESS : FBO CREATED" << std::endl;
        }
        else
        {
            std::cout << std::endl;
            std::cout << "ERROR : UNABLE TO CREATE FBO - [" << FBOError << "]" << std::endl;
        }

        selectedScene->FBOList.push_back(newFBO);

        std::cout << "SCENE CONVERTED TO FBO TEXTURE SUCCESSFULLY | SCENE ID : " << selectedScene->sceneID << " | FBO ID :" << FBOIndex << std::endl;
    }
}

void cSceneManager::AddDebugMeshToScene(unsigned int sceneId, cMesh* debugMesh, cDebugRenderer* debugRenderManager)
{
    if (this->mDebugRenderManager == NULL)
        this->mDebugRenderManager = debugRenderManager;

    if (sceneId < 0 || sceneId >= this->mSceneDetailsList.size())
    {
        std::cout << "ERROR : COULDN'T ADD DEDBUG MESH | SCENE ID PROVIDED DOES NOT MATCH" << std::endl;

        return;
    }

    sSceneDetails* selectedScene = this->mSceneDetailsList[sceneId];

    selectedScene->sceneDebugMeshList.push_back(debugMesh);
}

void cSceneManager::AddFBOTextureToMesh(unsigned int sceneIdOfFBOTexture, cMesh* meshNeedingTexture, unsigned int fboID)
{
    if (sceneIdOfFBOTexture >= this->mSceneDetailsList.size())
    {
        std::cout << "ERROR : COULDN'T ADD TEXTURE TO MESH | SCENE ID PROVIDED DOES NOT MATCH" << std::endl;

        return;
    }

    sSceneDetails* selectedScene = this->mSceneDetailsList[sceneIdOfFBOTexture];

    if (fboID >= selectedScene->FBOList.size())
    {
        std::cout << "ERROR : COULDN'T ADD TEXTURE TO MESH | FBO ID PROVIDED DOES NOT MATCH" << std::endl;

        return;
    }

    if (!selectedScene->bTurnIntoFBOTexture)
        ConvertSceneToFBOTexture(sceneIdOfFBOTexture, 1920, 1080);

    meshNeedingTexture->bTextureIsFromFBO = true;
    meshNeedingTexture->mMixedTextures.textureRatiosList[0] = 1.0f;
    meshNeedingTexture->FBOTextureNumber = selectedScene->FBOList[fboID]->colourTexture_0_ID;
}

void cSceneManager::AddSecondPassFilter(cMesh* meshTextureNeedingFilter, bool useFSQ, bool useStandardColor, bool useChromicAberration, bool UseScreenMeasurements, bool useNightVision)
{
    if (useChromicAberration)
    {
        meshTextureNeedingFilter->mSecondPassFilters.bUseChromicAberration = true;
        meshTextureNeedingFilter->mSecondPassFilters.bUseFSQ = useFSQ;
        meshTextureNeedingFilter->mSecondPassFilters.bUseNightVision = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseTextureWithScreenWidthAndHeight = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseStandardColorTexture = false;
    }
    else if (UseScreenMeasurements)
    {
        meshTextureNeedingFilter->mSecondPassFilters.bUseChromicAberration = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseFSQ = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseNightVision = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseTextureWithScreenWidthAndHeight = true;
        meshTextureNeedingFilter->mSecondPassFilters.bUseStandardColorTexture = false;
    }
    else if (useNightVision)
    {
        meshTextureNeedingFilter->mSecondPassFilters.bUseChromicAberration = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseFSQ = useFSQ;
        meshTextureNeedingFilter->mSecondPassFilters.bUseNightVision = true;
        meshTextureNeedingFilter->mSecondPassFilters.bUseTextureWithScreenWidthAndHeight = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseStandardColorTexture = false;
    }
    else
    {
        meshTextureNeedingFilter->mSecondPassFilters.bUseChromicAberration = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseFSQ = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseNightVision = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseTextureWithScreenWidthAndHeight = false;
        meshTextureNeedingFilter->mSecondPassFilters.bUseStandardColorTexture = true;
    }
}