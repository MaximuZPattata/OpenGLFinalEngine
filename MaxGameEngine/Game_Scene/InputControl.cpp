#include "pch.h"
#include "cControlGameEngine.h"
#include "sPlayerAttributes.h"
#include "cJsonWriter.h"
#include "sMouseAttributes.h"

//------------------------------INITIALIZE GLOBAL VARIABLES---------------------------------------------

extern cControlGameEngine gameEngine;

extern cJsonWriter jsonWriter;

extern sPlayerAttributes playerAttributes;

extern sMouseAttributes mouseAttributes;

//--------------------------------------RESET KEYS------------------------------------------------------

void ResetAllPlayerKeys()
{
    if (playerAttributes.mKeyPressedStateMap[GLFW_KEY_W] || playerAttributes.mKeyPressedStateMap[GLFW_KEY_S] ||
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_A] || playerAttributes.mKeyPressedStateMap[GLFW_KEY_D])
    {
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_W] = false;
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_S] = false;
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_A] = false;
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_D] = false;
    }

    if(playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT])
        playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT] = false;
}

//----------------------------------CALLBACK FUNCTIONS--------------------------------------------------

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    float ROTATION_DEGREES = 5.0f;

    const float CAMERA_SPEED = 300.0f;

    const float MODEL_MOVE_SPEED = 1.0f;

    float CAMERA_MOVEMENT = CAMERA_SPEED * static_cast<float>(gameEngine.deltaTime);

    glm::vec3 CAMERA_TARGET = gameEngine.GetCurrentCameraTarget();
    glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

    //----------------------------------------COMMON COMMANDS----------------------------------------------------------

    if (mods == 0)
    {
        if (key == GLFW_KEY_T && action == GLFW_PRESS) // Key to toggle between mesh bone coloring and mesh texture
        {
            gameEngine.ToggleMeshBoneWeightColor(playerAttributes.mPlayerModelName);
        }

        if (key == GLFW_KEY_L && action == GLFW_PRESS) // Key to write model enries to file
        {
            jsonWriter.WriteDataToTextFile(gameEngine);
        }

        if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
        {
            gameEngine.ShiftCameraView();

            if (gameEngine.IsFreeFlowCamOn()) // Make cursor visible and display the game world from the 1st camera showcase angle
            {
                CAMERA_POSITION = glm::vec3(0.0f, 50.0f, 300.0f);

                gameEngine.MoveCameraTarget(0.0f, 0.0f, -1.0f);
                gameEngine.SetCamPitch(-90.0f);
                gameEngine.SetCamPitch(0.f);
                mouseAttributes.resetMouseMoved();

                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

                if (playerAttributes.bPlayerRunAnimationLooped || playerAttributes.bPlayerWalkAnimationLooped)
                {
                    playerAttributes.bPlayerWalkAnimationLooped = false;
                    playerAttributes.bPlayerRunAnimationLooped = false;

                    ResetAllPlayerKeys();
                }

                gameEngine.SetEditorMode(true);
                gameEngine.SetLightEditorMode(false);
            }
            else // Make cursor invisible and recenter the cursor to the center of the screen
            {
                gameEngine.SetEditorMode(false);

                int screenWidth, screenHeight;

                glfwGetWindowSize(window, &screenWidth, &screenHeight);

                glfwSetCursorPos(window, screenWidth / 2, screenHeight / 2);

                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
        {
            if(gameEngine.GetEditorMode())
                gameEngine.SetLightEditorMode(!gameEngine.GetLightEditorMode());
        }
    }

    //-------------------------------------MESH CONTROL COMMANDS-------------------------------------------------------

    if (gameEngine.GetEditorMode() && !gameEngine.GetLightEditorMode())
    {
        if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
        {
           cMesh* controlMeshModel = gameEngine.GetCurrentModelSelected();

            std::string currentModelName = controlMeshModel->meshUniqueName;

            glm::vec3 currentModelPosition = gameEngine.GetModelPosition(currentModelName);

            float currentModelScaleValue = gameEngine.GetModelScaleValue(currentModelName);

            if (key == GLFW_KEY_W && action) // Decrement model's Z position
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y, currentModelPosition.z - MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_A && action) // Decrement model's X position
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x - MODEL_MOVE_SPEED, currentModelPosition.y, currentModelPosition.z);
            }
            if (key == GLFW_KEY_S && action) // Increment model's Z position
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y, currentModelPosition.z + MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_D && action) // Increment model's X position
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x + MODEL_MOVE_SPEED, currentModelPosition.y, currentModelPosition.z);
            }
            if (key == GLFW_KEY_Q && action) // Increment model's Y position 
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y + MODEL_MOVE_SPEED, currentModelPosition.z);
            }
            if (key == GLFW_KEY_E && action) // Decrement model's Y position
            {
                gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y - MODEL_MOVE_SPEED, currentModelPosition.z);
            }
            if (key == GLFW_KEY_I && action) // Rotate model's X axis - 5 degrees
            {
                gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 1.f, 0.f, 0.f);
            }
            if (key == GLFW_KEY_O && action) // Rotate model's Y axis - 5 degrees
            {
                gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 0.f, 1.f, 0.f);
            }
            if (key == GLFW_KEY_P && action) // Rotate model's Z axis - 5 degrees
            {
                gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 0.f, 0.f, 1.f);
            }
            if (key == GLFW_KEY_C && action) // Decrement model's scale value
            {
                gameEngine.ScaleModel(currentModelName, currentModelScaleValue - 0.01f);
            }
            if (key == GLFW_KEY_V && action) // Increment model's scale value
            {
                gameEngine.ScaleModel(currentModelName, currentModelScaleValue + 0.01f);
            }
            if (key == GLFW_KEY_Z && action) // Shift to previous model
            {
                gameEngine.ShiftToPreviousMeshInList();
            }
            if (key == GLFW_KEY_X && action) // Shift to next model
            {
                gameEngine.ShiftToNextMeshInList();
            }
            if (key == GLFW_KEY_B && action) // Change to wireframe mode
            {
                gameEngine.TurnWireframeModeOn(currentModelName);
            }
        }
    }

    //--------------------------------------CONTROLS FOR LIGHTS--------------------------------------------------------

    if (gameEngine.GetEditorMode() && gameEngine.GetLightEditorMode())
    {
        int lightId = gameEngine.GetCurrentLightSelected();

        if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
        {
            glm::vec3 lightPosition = gameEngine.GetLightPosition(lightId);
            float lightLinearAtten = gameEngine.GetLightLinearAttenuation(lightId);
            float lightQuadraticAtten = gameEngine.GetLightQuadraticAttenuation(lightId);
            float lightType = gameEngine.GetLightType(lightId);

            if (key == GLFW_KEY_W && action) // Decrement model's Z position
            {
                gameEngine.PositionLight(lightId, lightPosition.x, lightPosition.y, lightPosition.z - MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_A && action) // Decrement model's X position
            {
                gameEngine.PositionLight(lightId, lightPosition.x - MODEL_MOVE_SPEED, lightPosition.y, lightPosition.z);
            }
            if (key == GLFW_KEY_S && action) // Increment model's Z position
            {
                gameEngine.PositionLight(lightId, lightPosition.x, lightPosition.y, lightPosition.z + MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_D && action) // Increment model's X position
            {
                gameEngine.PositionLight(lightId, lightPosition.x + MODEL_MOVE_SPEED, lightPosition.y, lightPosition.z);
            }
            if (key == GLFW_KEY_Q && action) // Increment model's Y position 
            {
                gameEngine.PositionLight(lightId, lightPosition.x, lightPosition.y + MODEL_MOVE_SPEED, lightPosition.z);
            }
            if (key == GLFW_KEY_E && action) // Decrement model's Y position
            {
                gameEngine.PositionLight(lightId, lightPosition.x, lightPosition.y - MODEL_MOVE_SPEED, lightPosition.z);
            }
            if (key == GLFW_KEY_C && action) // Decrease quadratic atten
            {
                gameEngine.ChangeLightIntensity(lightId, lightLinearAtten, lightQuadraticAtten - 0.01f);
            }
            if (key == GLFW_KEY_V && action) // Increase quadratic atten
            {
                gameEngine.ChangeLightIntensity(lightId, lightLinearAtten, lightQuadraticAtten + 0.01f);
            }
            if (key == GLFW_KEY_Z && action) // Decrease linear atten
            {
                gameEngine.ChangeLightIntensity(lightId, lightLinearAtten - 0.01f, lightQuadraticAtten);
            }
            if (key == GLFW_KEY_X && action) // Increase linear atten
            {
                gameEngine.ChangeLightIntensity(lightId, lightLinearAtten + 0.01f, lightQuadraticAtten);
            }
            if (key == GLFW_KEY_B && action) // Change light Type
            {
                if (lightType == 2.0f)
                    lightType = 0.0f;
                else
                    lightType++;

                gameEngine.ChangeLightType(lightId, lightType);
            }
            if (key == GLFW_KEY_SPACE && action) // Shift to next light in list 
            {
                gameEngine.ShiftToNextLightInList();
            }
        }

        if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
        {
            glm::vec3 lightDirection = gameEngine.GetLightDirection(lightId);
            float lightInnerAngle = gameEngine.GetLightInnerAngle(lightId);
            float lightOuterAngle = gameEngine.GetLightOuterAngle(lightId);

            if (key == GLFW_KEY_W && action) // Decrement light's Z position
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x, lightDirection.y, lightDirection.z - MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_A && action) // Decrement light's X position
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x - MODEL_MOVE_SPEED, lightDirection.y, lightDirection.z);
            }
            if (key == GLFW_KEY_S && action) // Increment light's Z position
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x, lightDirection.y, lightDirection.z + MODEL_MOVE_SPEED);
            }
            if (key == GLFW_KEY_D && action) // Increment light's X position
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x + MODEL_MOVE_SPEED, lightDirection.y, lightDirection.z);
            }
            if (key == GLFW_KEY_Q && action) // Increment light's Y position 
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x, lightDirection.y + MODEL_MOVE_SPEED, lightDirection.z);
            }
            if (key == GLFW_KEY_E && action) // Decrement light's Y position
            {
                gameEngine.ChangeLightDirection(lightId, lightDirection.x, lightDirection.y - MODEL_MOVE_SPEED, lightDirection.z);
            }
            if (key == GLFW_KEY_C && action) // Decrease outer angle
            {
                gameEngine.ChangeLightAngle(lightId, lightInnerAngle, lightOuterAngle - 0.01f);
            }
            if (key == GLFW_KEY_V && action) // Increase outer angle
            {
                gameEngine.ChangeLightAngle(lightId, lightInnerAngle, lightOuterAngle + 0.01f);
            }
            if (key == GLFW_KEY_Z && action) // Decrease inner angle
            {
                gameEngine.ChangeLightAngle(lightId, lightInnerAngle - 0.01f, lightOuterAngle);
            }
            if (key == GLFW_KEY_X && action) // Increase inner angle
            {
                gameEngine.ChangeLightAngle(lightId, lightInnerAngle + 0.01f, lightOuterAngle);
            }
            if (key == GLFW_KEY_B && action) // Turn on/off light
            {
                if (gameEngine.IsLightOn(lightId))
                    gameEngine.TurnOffLight(lightId, true);
                else
                    gameEngine.TurnOffLight(lightId, false);
            }
        }
    }

    //------------------------------FREE FLOW CAM COMMANDS------------------------------------------

    if (mods == 0)
    {
        if (gameEngine.IsFreeFlowCamOn()) // Keys to be pressed when in free-flow cam mode
        {
            //------------------------------Camera Showcases------------------------------------------------

            if (key == GLFW_KEY_1 && action == GLFW_PRESS)
            {
                CAMERA_POSITION = glm::vec3(0.0f, 50.0f, 300.0f);
                gameEngine.MoveCameraTarget(0.0f, 0.0f, -1.0f);
                gameEngine.SetCamYaw(-90.0f);
                gameEngine.SetCamPitch(0.f);
                mouseAttributes.resetMouseMoved();
            }

            if (key == GLFW_KEY_2 && action == GLFW_PRESS)
            {
                CAMERA_POSITION = glm::vec3(-290.0f, 70.0f, 0.f);
                gameEngine.MoveCameraTarget(1.f, -0.06f, 0.03f);
                gameEngine.SetCamYaw(-358.241f);
                gameEngine.SetCamPitch(-3.65f);
                mouseAttributes.resetMouseMoved();
            }

            if (key == GLFW_KEY_3 && action == GLFW_PRESS)
            {
                CAMERA_POSITION = glm::vec3(-4.25f, 55.f, -280.f);
                gameEngine.MoveCameraTarget(-0.0115f, 0.009f, 1.0f);
                gameEngine.SetCamYaw(-269.345f);
                gameEngine.SetCamPitch(0.5f);
                mouseAttributes.resetMouseMoved();
            }

            if (key == GLFW_KEY_4 && action == GLFW_PRESS)
            {
                CAMERA_POSITION = glm::vec3(270.f, 53.f, 6.f);
                gameEngine.MoveCameraTarget(-1.f, -0.016f, -0.035f);
                gameEngine.SetCamYaw(-177.998f);
                gameEngine.SetCamPitch(-0.9f);
                mouseAttributes.resetMouseMoved();
            }

            if (key == GLFW_KEY_5 && action == GLFW_PRESS)
            {
                CAMERA_POSITION = glm::vec3(3.05f, 895.f, 200.f);
                gameEngine.MoveCameraTarget(-0.0012f, -0.98f, -0.22f);
                gameEngine.SetCamYaw(-90.3f);
                gameEngine.SetCamPitch(-77.55f);
                mouseAttributes.resetMouseMoved();
            }

            //------------------------------Move Camera--------------------------------------------------------

            if (key == GLFW_KEY_W && action)
            {
                CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_TARGET;
            }
            if (key == GLFW_KEY_A && action)
            {
                CAMERA_POSITION -= glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
            }
            if (key == GLFW_KEY_S && action)
            {
                CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_TARGET;
            }
            if (key == GLFW_KEY_D && action)
            {
                CAMERA_POSITION += glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
            }
            if (key == GLFW_KEY_Q && action)
            {
                CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_UP;
            }
            if (key == GLFW_KEY_E && action)
            {
                CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_UP;
            }
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) // Key to pause mouse movement
            {
                if (mouseAttributes.bEnableMouseMovement)
                    mouseAttributes.bEnableMouseMovement = false;
                else
                    mouseAttributes.bEnableMouseMovement = true;

                mouseAttributes.resetMouseMoved();
            }

            gameEngine.MoveCameraPosition(CAMERA_POSITION.x, CAMERA_POSITION.y, CAMERA_POSITION.z);
        }
    }

    //---------------------------------NON-EDOTOR MODE COMMANDS----------------------------------------------------

    if (!gameEngine.GetEditorMode())
    {
        //---------------------------------PLAYER COMMANDS----------------------------------------------

        if (!gameEngine.IsFreeFlowCamOn()) // Keys to be pressed when in 3rd person camera mode
        {
            if (key == GLFW_KEY_LEFT_SHIFT) // Run Key
            {
                if (action == GLFW_PRESS)
                {
                    if (!playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT])
                        playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT] = true;
                }
                if (action == GLFW_RELEASE)
                {
                    if (playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT])
                        playerAttributes.mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT] = false;
                }
            }

            if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D) // Directional Keys
            {
                if (action == GLFW_PRESS)
                {
                    playerAttributes.mKeyPressedStateMap[key] = true; // Switching the "Key Pressed" state to true
                }

                if (action == GLFW_RELEASE)
                {
                    //-------------------Reset Parameters------------------------------------------

                    playerAttributes.mKeyPressedStateMap[key] = false; // Switching the key pressed state to false

                    if (!playerAttributes.mKeyPressedStateMap[GLFW_KEY_W] && !playerAttributes.mKeyPressedStateMap[GLFW_KEY_S] &&
                        !playerAttributes.mKeyPressedStateMap[GLFW_KEY_A] && !playerAttributes.mKeyPressedStateMap[GLFW_KEY_D])
                    {
                        playerAttributes.bPlayerWalkAnimationLooped = false;
                        playerAttributes.bPlayerRunAnimationLooped = false;
                    }
                }
            }
        }
    }

    return;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    int screenWidth, screenHeight;

    glfwGetWindowSize(window, &screenWidth, &screenHeight);

    //if (mouseAttributes.bEnableMouseMovement)
    //{
    //    //------------------------Initializing Mouse Parameters--------------------------------------

    //    glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    //    glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

    //    float xOffset = 0.0f;
    //    float yOffset = 0.0f;

    //    //----------------Calculating Cursor Restrictions in X and Y Plane---------------------------

    //    if (xPos >= screenWidth - 1 || xPos <= 0) // Rotate Cam when mouse moves to the edge of the screen(left or right)
    //    {
    //        xOffset = 0.0f;

    //        if (xPos <= 0)
    //            xOffset = -10.0f;
    //        else if (xPos >= screenWidth - 1)
    //            xOffset = 10.0f;

    //        gameEngine.yaw += mouseAttributes.mouseSensitivity * xOffset;
    //    }

    //    if (yPos >= screenHeight - 1 || yPos <= 20) // Rotate Cam when mouse moves to the edge of the screen(up or down)
    //    {
    //        yOffset = 0.0f;

    //        if (yPos <= 20)
    //            yOffset = 10.0f;
    //        else if (yPos >= screenHeight - 1)
    //            yOffset = -10.0f;

    //        gameEngine.pitch += mouseAttributes.mouseSensitivity * yOffset;
    //    }

    //    //-----------------------Calculating the last X and Y Pos-------------------------------------

    //    if (mouseAttributes.bMouseMoved)
    //    {
    //        mouseAttributes.freeCamLastX = xPos;
    //        mouseAttributes.freeCamLastY = yPos;
    //        mouseAttributes.bMouseMoved = false;
    //    }

    //    //-------------------Calculating offsets with XY Coordinates----------------------------------

    //    xOffset = xPos - mouseAttributes.freeCamLastX;
    //    yOffset = mouseAttributes.freeCamLastY - yPos; //Reversed Y

    //    xOffset *= mouseAttributes.mouseSensitivity;
    //    yOffset *= mouseAttributes.mouseSensitivity;

    //    //----------------------Calculating Camera's Yaw and Pitch-----------------------------------

    //    gameEngine.yaw += xOffset;
    //    gameEngine.pitch += yOffset;

    //    //-------------------------Restricting Vertical Movements------------------------------------

    //    if (gameEngine.pitch > 90.0f)
    //        gameEngine.pitch = 90.0f;

    //    if (gameEngine.pitch < -90.0f)
    //        gameEngine.pitch = -90.0f;

    //    //----------------------------Calculating Camera Front---------------------------------------

    //    glm::vec3 front;

    //    front.x = cos(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));
    //    front.y = sin(glm::radians(gameEngine.pitch));
    //    front.z = sin(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));

    //    glm::vec3 cameraFront = glm::normalize(front);

    //    gameEngine.MoveCameraTarget(cameraFront.x, cameraFront.y, cameraFront.z);

    //    //-------------------------Updating Last Mouse Position--------------------------------------

    //    mouseAttributes.freeCamLastX = xPos;
    //    mouseAttributes.freeCamLastY = yPos;
    //}


    // Free-Flow Camera View
    if (gameEngine.IsFreeFlowCamOn())
    {
        if (mouseAttributes.bEnableMouseMovement)
        {
            //------------------------Initializing Mouse Parameters--------------------------------------

            glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

            float xOffset = 0.f;
            float yOffset = 0.f;
            float camYaw = gameEngine.GetCamYaw();
            float camPitch = gameEngine.GetCamPitch();

            //----------------Calculating Cursor Restrictions in X and Y Plane---------------------------

            if (xPos >= screenWidth - 1 || xPos <= 0) // Rotate Cam when mouse moves to the edge of the screen(left or right)
            {
                xOffset = 0.0f;

                if (xPos <= 0)
                    xOffset = -10.0f;
                else if (xPos >= screenWidth - 1)
                    xOffset = 10.0f;

                camYaw += mouseAttributes.mouseSensitivity * xOffset;
            }

            if (yPos >= screenHeight - 1 || yPos <= 20) // Rotate Cam when mouse moves to the edge of the screen(up or down)
            {
                yOffset = 0.0f;

                if (yPos <= 20)
                    yOffset = 10.0f;
                else if (yPos >= screenHeight - 1)
                    yOffset = -10.0f;

                camPitch += mouseAttributes.mouseSensitivity * yOffset;
            }

            //-----------------------Calculating the last X and Y Pos-------------------------------------

            if (mouseAttributes.bMouseMoved)
            {
                mouseAttributes.freeCamLastX = xPos;
                mouseAttributes.freeCamLastY = yPos;
                mouseAttributes.bMouseMoved = false;
            }

            //-------------------Calculating offsets with XY Coordinates----------------------------------

            xOffset = xPos - mouseAttributes.freeCamLastX;
            yOffset = mouseAttributes.freeCamLastY - yPos; //Reversed Y

            xOffset *= mouseAttributes.mouseSensitivity;
            yOffset *= mouseAttributes.mouseSensitivity;

            //----------------------Calculating Camera's Yaw and Pitch-----------------------------------

            camYaw += xOffset;
            camPitch += yOffset;

            //-------------------------Restricting Vertical Movements------------------------------------

            if (camPitch > 90.0f)
                camPitch = 90.0f;

            if (camPitch < -90.0f)
                camPitch = -90.0f;

            //----------------------------Calculating Camera Front---------------------------------------

            glm::vec3 front;

            front.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
            front.y = sin(glm::radians(camPitch));
            front.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));

            glm::vec3 cameraFront = glm::normalize(front);

            gameEngine.MoveCameraTarget(cameraFront.x, cameraFront.y, cameraFront.z);
            gameEngine.SetCamYaw(camYaw);
            gameEngine.SetCamPitch(camPitch);

            //-------------------------Updating Last Mouse Position--------------------------------------

            mouseAttributes.freeCamLastX = xPos;
            mouseAttributes.freeCamLastY = yPos;
        }
    }

    // Player's 3rd person view
    else
    {
        //-----------------------Calculating the last X and Y Pos------------------------------------

        mouseAttributes.TPV_LastX = screenWidth / 2;
        mouseAttributes.TPV_LastY = screenHeight / 2;

        //-------------------Calculating offsets with XY Coordinates---------------------------------

        double xOffset = screenWidth / 2 - xPos;
        double yOffset = screenHeight / 2 - yPos;

        //----------------------Calculating Player's Yaw and Pitch-----------------------------------

        playerAttributes.mPlayerYaw += xOffset * mouseAttributes.mouseSensitivity * 2.5f;
        playerAttributes.mPlayerPitch += yOffset * mouseAttributes.mouseSensitivity;

        //------------------------Allowing 360 Horizontal Movements----------------------------------

        if (playerAttributes.mPlayerYaw > mouseAttributes.TPV_maxYaw)
            playerAttributes.mPlayerYaw -= mouseAttributes.TPV_maxYaw;

        else if (playerAttributes.mPlayerYaw < mouseAttributes.TPV_minYaw)
            playerAttributes.mPlayerYaw += mouseAttributes.TPV_maxYaw;

        //-------------------------Restricting Vertical Movements------------------------------------

        if (playerAttributes.mPlayerPitch > mouseAttributes.TPV_maxPitch)
            playerAttributes.mPlayerPitch = mouseAttributes.TPV_maxPitch;

        else if (playerAttributes.mPlayerPitch < -mouseAttributes.TPV_maxPitch)
            playerAttributes.mPlayerPitch = -mouseAttributes.TPV_maxPitch;

        //------------------Recenter the Cursor to the Center of the Screen--------------------------

        glfwSetCursorPos(window, screenWidth / 2, screenHeight / 2);

        //-------------------------Updating Last Mouse Position--------------------------------------

        mouseAttributes.TPV_LastX = screenWidth / 2;
        mouseAttributes.TPV_LastY = screenHeight / 2;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Scroll to zoom in 3rd person cam view
    if (!gameEngine.IsFreeFlowCamOn())
    {
        float camDistance = gameEngine.GetCamDistance();
        camDistance -= yoffset * mouseAttributes.scrollSpeed;
        camDistance = glm::clamp(camDistance, mouseAttributes.minZoomDistance, mouseAttributes.maxZoomDistance);

        gameEngine.SetCameraDistance(camDistance);
    }
}

