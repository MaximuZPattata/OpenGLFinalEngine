#include "pch.h"
#include <cControlGameEngine.h>

void CreateAllSoftBodies(cControlGameEngine& gameEngine)
{
    std::string softBodyMeshName01 = "Chain_Piece_01";

    gameEngine.CreateVerletSoftBody(softBodyMeshName01, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName01, 100, 1.f);
    // gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName01);
    gameEngine.FixSoftBodyPosition(softBodyMeshName01, glm::vec3(-120.f, 120.f, 0.f));


    std::string softBodyMeshName02 = "Chain_Piece_02";

    gameEngine.CreateVerletSoftBody(softBodyMeshName02, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName02, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName02);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName01, softBodyMeshName02);

    std::string softBodyMeshName03 = "Chain_Piece_03";

    gameEngine.CreateVerletSoftBody(softBodyMeshName03, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName03, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName03);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName02, softBodyMeshName03);

    std::string softBodyMeshName04 = "Chain_Piece_04";

    gameEngine.CreateVerletSoftBody(softBodyMeshName04, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName04, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName04);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName03, softBodyMeshName04);


    std::string softBodyMeshName05 = "Chain_Piece_05";

    gameEngine.CreateVerletSoftBody(softBodyMeshName05, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName05, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName05);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName04, softBodyMeshName05);

    std::string cubeMesh01 = "Cube_01";

    gameEngine.CreateVerletSoftBody(cubeMesh01, glm::vec3(0.0f, -9.8f, 0.0f), false, false, false, true, false);
    gameEngine.ConnectTwoSoftBodies(softBodyMeshName05, cubeMesh01);

    std::string softBodyMeshName06 = "Chain_Piece_06";

    gameEngine.CreateVerletSoftBody(softBodyMeshName06, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName06, 100, 1.f);
    // gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName06);
    gameEngine.FixSoftBodyPosition(softBodyMeshName06, glm::vec3(120.f, 120.f, 0.f));


    std::string softBodyMeshName07 = "Chain_Piece_07";

    gameEngine.CreateVerletSoftBody(softBodyMeshName07, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName07, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName07);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName06, softBodyMeshName07);

    std::string softBodyMeshName08 = "Chain_Piece_08";

    gameEngine.CreateVerletSoftBody(softBodyMeshName08, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName08, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName08);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName07, softBodyMeshName08);

    std::string softBodyMeshName09 = "Chain_Piece_09";

    gameEngine.CreateVerletSoftBody(softBodyMeshName09, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName09, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName09);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName08, softBodyMeshName09);


    std::string softBodyMeshName010 = "Chain_Piece_10";

    gameEngine.CreateVerletSoftBody(softBodyMeshName010, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName010, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName010);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName09, softBodyMeshName010);

    std::string cubeMesh02 = "Cube_02";

    gameEngine.CreateVerletSoftBody(cubeMesh02, glm::vec3(0.0f, -9.8f, 0.0f), false, false, false, true, false);
    gameEngine.ConnectTwoSoftBodies(softBodyMeshName010, cubeMesh02);

    std::string softBodyMeshName11 = "Chain_Piece_11";

    gameEngine.CreateVerletSoftBody(softBodyMeshName11, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName11, 100, 1.f);
    // gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName11);
    gameEngine.FixSoftBodyPosition(softBodyMeshName11, glm::vec3(-60.f, 120.f, -60.f));


    std::string softBodyMeshName12 = "Chain_Piece_12";

    gameEngine.CreateVerletSoftBody(softBodyMeshName12, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName12, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName12);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName11, softBodyMeshName12);

    std::string softBodyMeshName13 = "Chain_Piece_13";

    gameEngine.CreateVerletSoftBody(softBodyMeshName13, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName13, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName13);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName12, softBodyMeshName13);

    std::string softBodyMeshName14 = "Chain_Piece_14";

    gameEngine.CreateVerletSoftBody(softBodyMeshName14, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName14, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName14);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName13, softBodyMeshName14);


    std::string softBodyMeshName15 = "Chain_Piece_15";

    gameEngine.CreateVerletSoftBody(softBodyMeshName15, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName15, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName15);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName14, softBodyMeshName15);

    std::string cubeMesh03 = "Cube_03";

    gameEngine.CreateVerletSoftBody(cubeMesh03, glm::vec3(0.0f, -9.8f, 0.0f), false, false, false, true, false);
    gameEngine.ConnectTwoSoftBodies(softBodyMeshName15, cubeMesh03);

    std::string softBodyMeshName16 = "Chain_Piece_16";

    gameEngine.CreateVerletSoftBody(softBodyMeshName16, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName16, 100, 1.f);
    // gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName16);
    gameEngine.FixSoftBodyPosition(softBodyMeshName16, glm::vec3(60.f, 120.f, -60.f));


    std::string softBodyMeshName17 = "Chain_Piece_17";

    gameEngine.CreateVerletSoftBody(softBodyMeshName17, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName17, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName17);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName16, softBodyMeshName17);

    std::string softBodyMeshName18 = "Chain_Piece_18";

    gameEngine.CreateVerletSoftBody(softBodyMeshName18, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName18, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName18);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName17, softBodyMeshName18);

    std::string softBodyMeshName19 = "Chain_Piece_19";

    gameEngine.CreateVerletSoftBody(softBodyMeshName19, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName19, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName19);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName18, softBodyMeshName19);


    std::string softBodyMeshName20 = "Chain_Piece_20";

    gameEngine.CreateVerletSoftBody(softBodyMeshName20, glm::vec3(0.0f, -9.8f, 0.0f), false, false, true, false, false);
    gameEngine.ApplyRandomBracingToSoftBody(softBodyMeshName20, 100, 1.f);
    //gameEngine.ApplyImaginaryCenterNodeBracing(softBodyMeshName20);

    gameEngine.ConnectTwoSoftBodies(softBodyMeshName19, softBodyMeshName20);

    std::string cubeMesh04 = "Cube_04";

    gameEngine.CreateVerletSoftBody(cubeMesh04, glm::vec3(0.0f, -9.8f, 0.0f), false, false, false, true, false);
    gameEngine.ConnectTwoSoftBodies(softBodyMeshName20, cubeMesh04);

    std::string ballModelName = "Ball";

    gameEngine.CreateVerletSoftBody(ballModelName, glm::vec3(0.0f, -9.8f, 0.0f), true, true, false, false, true);
    gameEngine.ApplyRandomBracingToSoftBody(ballModelName, 200, 1.f);
    //gameEngine.TurnVisibilityOn(ballModelName);
    gameEngine.ApplyImaginaryCenterNodeBracing(ballModelName);

}

void CreateThreadedSoftBodies(cControlGameEngine& gameEngine)
{
    bool createDebugSpheres = false; // Not threaded, so better to keep it off. But works better when the soft body count is lower( < 5 )
    bool createSoftBodyThreads = true;

    unsigned int TOTAL_COUNT = 81;

    std::string modelName = "";
    std::string debugModelName = "";

    for (int count = 1; count <= TOTAL_COUNT; count++)
    {
        modelName = "Blob0" + std::to_string(count);
        debugModelName = "DebugSphereVerletCloth0" + std::to_string(count);

        gameEngine.CreateVerletSoftBodyWithThread(modelName, glm::vec3(0.0f, -15.0f, 0.0f), createDebugSpheres, debugModelName, 0.25f, glm::vec4(0.1, 0.1, 0.1, 1.0), createSoftBodyThreads);
        gameEngine.ApplyRandomBracingToSoftBody(modelName, 300, 1.f);
    }
}

void CreateFBOScene(cControlGameEngine& gameEngine)
{
    gameEngine.AdjustAlphaTransparency("Water", 0.3f);

    gameEngine.RenderMeshWithFBOTexture(0, "Central_Console_Screen_01", 0);
    gameEngine.RenderMeshWithFBOTexture(0, "Central_Console_Screen_03", 1);
    gameEngine.AddSecondPassFilterToScene("Central_Console_Screen_01", false, false, true, false, false);
    gameEngine.AddSecondPassFilterToScene("Central_Console_Screen_03", false, false, false, false, true);
}