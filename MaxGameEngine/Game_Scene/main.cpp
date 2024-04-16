#include "pch.h"
#include "cMesh.h"
#include "cJsonReader.h"
#include "cJsonWriter.h"
#include "cHiResTimer.h"
#include "sPlayerAttributes.h"
#include "sMouseAttributes.h"
#include "cAnimationSystem.h"

#include <cControlGameEngine.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//---------------------------Global Objects-----------------------------------------------

GLFWwindow* window;
cControlGameEngine gameEngine;
cJsonReader jsonReader;
cJsonWriter jsonWriter;
sCubeMapDetailsFromFile cubeMapDetails;
sPlayerAttributes playerAttributes;
sMouseAttributes mouseAttributes;
cHiResTimer* timer = new cHiResTimer(60);

std::vector<sSceneDetailsFromFile> sceneDetailsList; 
std::vector<sModelDetailsFromFile> modelDetailsList;
std::vector<sLightDetailsFromFile> lightDetailsList;
std::vector<sPhysicsDetailsFromFile> physicsDetailsList;

const float POSITION_OFFSET = 0.0f;

extern void CreateAllSoftBodies(cControlGameEngine& engineManager);
extern void CreateThreadedSoftBodies(cControlGameEngine& gameEngine);
extern void CreateFBOScene(cControlGameEngine& gameEngine);

void UpdatePerFrame();

int main()
{
    //-----------------------------------Initialize Window--------------------------------------

    int result = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1200, 800, "Game World", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //------------------------------Input key and Cursor initialize-----------------------------

    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //--------------------------------Initialize Game Engine----------------------------------------

    result = gameEngine.InitializeGameEngine();

    if (result != 0)
        return -1;

    //--------------------------------Loading Models, Lights and initial camera position from Json file---------------------------------------------

    bool jsonresult = jsonReader.ReadScene("SceneDescription.json", sceneDetailsList, modelDetailsList, cubeMapDetails, physicsDetailsList, lightDetailsList);

    if (jsonresult)
    {
        std::cout << "File read successfully !" << std::endl;

        std::string modelName;
        float angleDegrees;

        //-----------------------------------Adding Scenes--------------------------------------------------------------------------------

        for (int index = 0; index < sceneDetailsList.size(); index++)
        {
            if (sceneDetailsList[index].bIsMainScene)
            {
                if (sceneDetailsList[index].sceneCameraPositionList.size() > 1)
                {
                    std::cout << "ERROR : MAIN SCENE SHOULDN'T HAVE MORE THAN ONE CAMERA ANGLE" << std::endl;
                    return -1;
                }
                else
                {
                    glm::vec3 camPos = sceneDetailsList[index].sceneCameraPositionList[0];
                    glm::vec3 camTarget = sceneDetailsList[index].sceneCameraTargetList[0];

                    gameEngine.MoveCameraPosition(camPos.x + POSITION_OFFSET, camPos.y + POSITION_OFFSET, camPos.z + POSITION_OFFSET);
                }
            }

            gameEngine.CreateANewScene(sceneDetailsList[index].bIsMainScene, sceneDetailsList[index].sceneCameraPositionList, sceneDetailsList[index].sceneCameraTargetList);

            if (sceneDetailsList[index].bIsFBOTexture)
                gameEngine.MakeSceneIntoFBOTexture(sceneDetailsList[index].sceneId, sceneDetailsList[index].windowWidth, sceneDetailsList[index].windowHeight);
        }

        //-----------------------------------Adding Models--------------------------------------------------------------------------------

        for (int index = 0; index < modelDetailsList.size(); index++)
        {
            modelName = modelDetailsList[index].modelName;

            //---------------------------Load Models with position----------------------------------------------------------------------

            gameEngine.LoadModelsInto3DSpace(modelDetailsList[index].modelFilePath, modelName, modelDetailsList[index].modelPosition.x + POSITION_OFFSET,
                modelDetailsList[index].modelPosition.y + POSITION_OFFSET, modelDetailsList[index].modelPosition.z + POSITION_OFFSET);

            //---------------------------------Rotate Models-----------------------------------------------------------------------------

            if (modelDetailsList[index].modelOrientation.x != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.x;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 1.f, 0.f, 0.f);
            }
            if (modelDetailsList[index].modelOrientation.y != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.y;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 0.f, 1.f, 0.f);
            }
            if (modelDetailsList[index].modelOrientation.z != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.z;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 0.f, 0.f, 1.f);
            }

            //------------------------------------Scale Models-----------------------------------------------------------------------------

            gameEngine.ScaleModel(modelName, modelDetailsList[index].modelScaleValue);

            //-----------------------------------Model Wireframe---------------------------------------------------------------------------

            if (modelDetailsList[index].wireframeModeOn)
                gameEngine.TurnWireframeModeOn(modelName);

            //----------------------------------Model Mesh Light---------------------------------------------------------------------------

            if (modelDetailsList[index].meshLightsOn)
                gameEngine.TurnMeshLightsOn(modelName);

            //-------------------------------------Model Bones-----------------------------------------------------------------------------

            if (modelDetailsList[index].applyBones)
                gameEngine.ApplyBonesToMesh(modelName, true);

            //----------------------------------Cube Map Details-----------------------------------------------------------------------------

            if (modelDetailsList[index].isSkyBox)
            {
                if (!cubeMapDetails.cubeMapName.empty())
                {
                    result = gameEngine.AddCubeMap(modelName, cubeMapDetails.cubeMapName, cubeMapDetails.filePathPosX, cubeMapDetails.filePathNegX,
                        cubeMapDetails.filePathPosY, cubeMapDetails.filePathNegY, cubeMapDetails.filePathPosZ,
                        cubeMapDetails.filePathNegZ);
                    if (result != 0)
                        return -1;
                }
                else
                {
                    std::cout << "Cube Properties wasnt updated or added in the json file" << std::endl;

                    return -1;
                }
            }
            else
            {
                //------------------------------------Color Models-----------------------------------------------------------------------------

                if (modelDetailsList[index].manualColors)
                {
                    gameEngine.UseManualColors(modelName, true);
                    gameEngine.ChangeColor(modelName, modelDetailsList[index].modelColorRGB.x, modelDetailsList[index].modelColorRGB.y, modelDetailsList[index].modelColorRGB.z);
                }

                //------------------------------------Add Textures------------------------------------------------------------------------------

                if (modelDetailsList[index].useTextures)
                {
                    gameEngine.UseTextures(modelName, true);

                    for (int textureIndex = 0; textureIndex < modelDetailsList[index].textureNameList.size(); textureIndex++)
                    {
                        if (modelDetailsList[index].blendTexture[textureIndex])
                            gameEngine.AddTexturesToTheMix(modelName, modelDetailsList[index].textureFilePathList[textureIndex],
                                modelDetailsList[index].textureNameList[textureIndex], modelDetailsList[index].textureRatiosList[textureIndex]);
                        else
                            gameEngine.AddTexturesToOverlap(modelName, modelDetailsList[index].textureFilePathList[textureIndex],
                                modelDetailsList[index].textureNameList[textureIndex]);

                        if (modelDetailsList[index].useDiscardMaskTexture[textureIndex])
                            gameEngine.AddDiscardMaskTexture(modelName, modelDetailsList[index].textureNameList[textureIndex],
                                modelDetailsList[index].discardMaskTextureFilePath[textureIndex]);
                    }
                }
            }

            //--------------------------------Add Model into Scene--------------------------------------------------------------------------

            gameEngine.AddMeshToExistingScene(modelDetailsList[index].meshSceneId, modelDetailsList[index].modelName);

            //-----------------------------------Adding Physics------------------------------------------------------------------------------

            if (modelDetailsList[index].physicsMeshType == "Sphere") // Sphere Physics
            {
                for (int physicsIndex = 0; physicsIndex < physicsDetailsList.size(); physicsIndex++)
                {
                    if (physicsDetailsList[physicsIndex].modelName == modelName)
                    {
                        gameEngine.AddSpherePhysicsToMesh(modelName, modelDetailsList[index].physicsMeshType, physicsDetailsList[physicsIndex].modelRadius);

                        if (physicsDetailsList[physicsIndex].randomVelocity)
                        {
                            float randomVelocity = gameEngine.getRandomFloat(0.7, 5.0);

                            gameEngine.ChangeModelPhysicsVelocity(modelName, glm::vec3(0.0f, -5.0f, 0.0f));
                        }
                        else
                            gameEngine.ChangeModelPhysicsVelocity(modelName, glm::vec3(physicsDetailsList[physicsIndex].modelVelocity.x, physicsDetailsList[physicsIndex].modelVelocity.y,
                                physicsDetailsList[physicsIndex].modelVelocity.z));

                        gameEngine.ChangeModelPhysicsAcceleration(modelName, glm::vec3(physicsDetailsList[physicsIndex].modelAcceleration.x, physicsDetailsList[physicsIndex].modelAcceleration.y,
                            physicsDetailsList[physicsIndex].modelAcceleration.z));

                        result = gameEngine.ChangeModelPhysicalMass(modelName, physicsDetailsList[physicsIndex].modelMass);

                        if (result != 0)
                            std::cout << "Mass provided for the sphere - '" << modelName << "' is not above 0. Hence default mass 1.0/10.0 is used for this sphere model." << std::endl;
                    }
                }
            }
            else if (modelDetailsList[index].physicsMeshType == "Plane" || modelDetailsList[index].physicsMeshType == "Box") // Plane Physics
            {
                gameEngine.AddPlanePhysicsToMesh(modelName, modelDetailsList[index].physicsMeshType);
            }
        }

        //-----------------------------------Adding Lights--------------------------------------------------------------------------------

        for (int index = 0; index < lightDetailsList.size(); index++)
        {
            glm::vec4 lightPos = glm::vec4(lightDetailsList[index].lightPosition.x, lightDetailsList[index].lightPosition.y, lightDetailsList[index].lightPosition.z, 1.f);
            glm::vec4 lightDir = glm::vec4(lightDetailsList[index].lightDirection.x, lightDetailsList[index].lightDirection.y, lightDetailsList[index].lightDirection.z, 1.f);
            glm::vec2 lightAngle = glm::vec2(lightDetailsList[index].innerAngle, lightDetailsList[index].outerAngle);

            gameEngine.AddLight(lightPos, lightDir, lightDetailsList[index].lightOff, lightDetailsList[index].lightType,
                lightDetailsList[index].linearAttenuation, lightDetailsList[index].quadraticAttenuation, lightAngle, lightDetailsList[index].lightColorRGB);
        }
    }
    else
        return -1;

    //-------------------------Loading animation onto mesh----------------------------------

    std::string playerModelName = "Vampire";
    std::string runAnimName = "Vampire_Run";
    std::string walkAnimName = "Vampire_Walk";
    std::string idleAnimName = "Vampire_Idle";
    std::string jumpAnimName = "Vampire_Jump";

    gameEngine.LoadAnimationIntoExistingMeshModel(playerModelName, "Run.dae", runAnimName);
    gameEngine.LoadAnimationIntoExistingMeshModel(playerModelName, "Standard Walk.dae", walkAnimName);
    gameEngine.LoadAnimationIntoExistingMeshModel(playerModelName, "Jumping.dae", jumpAnimName);
    gameEngine.LoadAnimationIntoExistingMeshModel(playerModelName, "Breathing_Idle.dae", idleAnimName);

    //------------------------------Creating Scene-------------------------------------------

    //CreateThreadedSoftBodies(gameEngine);

    //CreateAllSoftBodies(gameEngine);

    //CreateFBOScene(gameEngine);

    playerAttributes.InitializePlayer(gameEngine, playerModelName, idleAnimName, walkAnimName, runAnimName, 25.f, 50.f, 10.f, 75.f, mouseAttributes.minZoomDistance);

    //-------------------------------Frame loop----------------------------------------------

    const double LARGEST_DELTA_TIME = 1.0f / 30.0f;
    double lastTime = glfwGetTime();
    float ONE_SEC_COUNT = 0.0f;
    float FPS = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        //----------------------Frame Rate Count---------------------------------------------

        gameEngine.deltaTime = timer->getFrameTime();

        ONE_SEC_COUNT += static_cast<float>(gameEngine.deltaTime);

        if (ONE_SEC_COUNT >= 0.99f)
        {
            std::cout << "FPS : " << FPS << " | SECONDS_TAKEN : " << ONE_SEC_COUNT << std::endl;
            FPS = 1.0f;
            ONE_SEC_COUNT = 0.0f;
        }
        else
            FPS++;

        //------------------Update animations-------------------------------------------------

        UpdatePerFrame();

        //--------------------Run Engine-----------------------------------------------------

        gameEngine.RunGameEngine(window);
    }

    glfwDestroyWindow(window);

    gameEngine.TerminateSoftBodyThreads();

    glfwTerminate();

    exit(EXIT_SUCCESS);

    std::cout << "Window closed !" << std::endl;

    return 0;
}

void UpdatePerFrame()
{
    //gameEngine.AnimateChromicAmount("Central_Console_Screen_01", 0.005f);

    //gameEngine.UpdateSoftBodyTransformation();

    gameEngine.UpdateThirdPersonCamera(playerAttributes.mPlayerPosition, playerAttributes.mPlayerYaw, playerAttributes.mPlayerPitch);

    playerAttributes.UpdatePlayerState(gameEngine);

    gameEngine.UpdateAllAnimations(static_cast <float> (gameEngine.deltaTime));
}