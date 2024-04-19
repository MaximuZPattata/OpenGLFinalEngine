#include "pch.h"
#include "sPlayerAttributes.h"

#include <cmath>

void sPlayerAttributes::InitializePlayer(cControlGameEngine& gameEngine, std::string playerModelName, std::string idleAnimationName, std::string walkAnimationName,
    std::string runAnimationName, float walkSpeed, float runSpeed, float rotationSpeed, float camHeight, float camDistance)
{
    this->mPlayerModelName = playerModelName;
    this->mRunAnimationName = runAnimationName;
    this->mWalkAnimationName = walkAnimationName;
    this->mIdleAnimationName = idleAnimationName;

    this->mPlayerPosition = gameEngine.GetModelPosition(this->mPlayerModelName);
    this->mPlayerVelocity = glm::vec3(0.f);
    this->mPlayerWalkSpeed = walkSpeed;
    this->mPlayerRunSpeed = runSpeed;
    this->mPlayerRotationSpeed = rotationSpeed;
    this->mPlayerYaw = 0.f;
    this->mPlayerForward = glm::vec3(0.f, 0.f, 1.f);

    gameEngine.SetCameraHeight(this->mPlayerPosition.y + camHeight);
    gameEngine.SetCameraDistance(camDistance);
}

void sPlayerAttributes::AnimatePlayerWalk(cControlGameEngine& gameEngine)
{
    if (this->bPlayerIdleAnimationLooped)
        this->bPlayerIdleAnimationLooped = false;

    if (this->bPlayerRunAnimationLooped)
        this->bPlayerRunAnimationLooped = false;

    this->bPlayerWalkAnimationLooped = true;

    gameEngine.PlayAnimation(this->mPlayerModelName, this->mWalkAnimationName, 0.f, true, true);
}

void sPlayerAttributes::AnimatePlayerRun(cControlGameEngine& gameEngine)
{
    if (this->bPlayerIdleAnimationLooped)
        this->bPlayerIdleAnimationLooped = false;
    
    if(this->bPlayerWalkAnimationLooped)
        this->bPlayerWalkAnimationLooped = false;
    
    this->bPlayerRunAnimationLooped = true;

    gameEngine.PlayAnimation(this->mPlayerModelName, this->mRunAnimationName, 0.f, true, true);
}

void sPlayerAttributes::AnimatePlayerIdle(cControlGameEngine& gameEngine)
{
    this->bPlayerIdleAnimationLooped = true;

    gameEngine.PlayAnimation(this->mPlayerModelName, this->mIdleAnimationName, 0.f, true, true);
}

glm::vec3 sPlayerAttributes::GetDirectionToLook()
{
    float lookDistance = 200.f;

    glm::vec3 targetToLookAt = glm::vec3(0.f);

    //---------------Checking which direction user wants the player to turn------------------------------
    
    targetToLookAt = this->mPlayerPosition + this->mPlayerMovingDirection * lookDistance;

    //---------------Normalizing the direction to look at and returning it-------------------------------

    return glm::normalize(targetToLookAt - this->mPlayerPosition);
}

void sPlayerAttributes::UpdatePlayerState(cControlGameEngine& gameEngine)
{
    //----------------------------------Update Key Pressed State--------------------------------------------------------

    this->UpdateKeyPressedStateForPlayerControl(gameEngine);

    //-----------------------------------Update Player Position---------------------------------------------------------

    this->UpdatePlayerPosition(gameEngine);
}

void sPlayerAttributes::UpdateKeyPressedStateForPlayerControl(cControlGameEngine& gameEngine)
{
    if (this->mKeyPressedStateMap[GLFW_KEY_W] || this->mKeyPressedStateMap[GLFW_KEY_A] ||
        this->mKeyPressedStateMap[GLFW_KEY_S] || this->mKeyPressedStateMap[GLFW_KEY_D])
    {
        float forwardX = -sin(glm::radians(this->mPlayerYaw));
        float forwardZ = -cos(glm::radians(this->mPlayerYaw));

        glm::vec3 forwardDirection = glm::vec3(forwardX, 0.0f, forwardZ);

        float movementSpeed = 0.f;

        if (this->mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT])
            movementSpeed = this->mPlayerRunSpeed;
        else
            movementSpeed = this->mPlayerWalkSpeed;

        if (this->mKeyPressedStateMap[GLFW_KEY_W] && this->mKeyPressedStateMap[GLFW_KEY_D])
        {
            //-----------------Rotate and Move Player---------------------------------------

            this->mPlayerMovingDirection = glm::normalize(forwardDirection + glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * movementSpeed;

            this->mPlayerVelocity = this->mPlayerMovingDirection; // Move forward-right
        }
        else if (this->mKeyPressedStateMap[GLFW_KEY_W] && this->mKeyPressedStateMap[GLFW_KEY_A])
        {
            //-----------------Rotate and Move Player---------------------------------------

            this->mPlayerMovingDirection = glm::normalize(forwardDirection - glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * movementSpeed;

            this->mPlayerVelocity = this->mPlayerMovingDirection; // Move forward-left
        }
        else if (this->mKeyPressedStateMap[GLFW_KEY_S] && this->mKeyPressedStateMap[GLFW_KEY_A])
        {
            //-----------------Rotate and Move Player---------------------------------------

            this->mPlayerMovingDirection = glm::normalize(-forwardDirection - glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * movementSpeed;

            this->mPlayerVelocity = this->mPlayerMovingDirection; // Move backward-left
        }
        else if (this->mKeyPressedStateMap[GLFW_KEY_S] && this->mKeyPressedStateMap[GLFW_KEY_D])
        {
            //-----------------Rotate and Move Player---------------------------------------

            this->mPlayerMovingDirection = glm::normalize(-forwardDirection + glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * movementSpeed;

            this->mPlayerVelocity = this->mPlayerMovingDirection; // Move backward-right
        }
        else
        {
            if (this->mKeyPressedStateMap[GLFW_KEY_W])
            {
                //-----------------Rotate and Move Player---------------------------------------

                this->mPlayerMovingDirection = forwardDirection * movementSpeed;

                this->mPlayerVelocity = this->mPlayerMovingDirection; // Move forward
            }

            if (this->mKeyPressedStateMap[GLFW_KEY_S])
            {
                //-----------------Rotate and Move Player---------------------------------------

                this->mPlayerMovingDirection = -forwardDirection * movementSpeed;

                this->mPlayerVelocity = this->mPlayerMovingDirection; // Move backward
            }

            if (this->mKeyPressedStateMap[GLFW_KEY_A])
            {
                //-----------------Rotate and Move Player---------------------------------------

                this->mPlayerMovingDirection = -glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f)) * movementSpeed;

                this->mPlayerVelocity = this->mPlayerMovingDirection; // Move left
            }

            if (this->mKeyPressedStateMap[GLFW_KEY_D])
            {
                //-----------------Rotate and Move Player---------------------------------------

                this->mPlayerMovingDirection = glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f)) * movementSpeed;

                this->mPlayerVelocity = this->mPlayerMovingDirection; // Move right
            }
        }

        //--------------------Animate Player-------------------------------------------

        if (this->mKeyPressedStateMap[GLFW_KEY_LEFT_SHIFT])
        {
            if (!this->bPlayerRunAnimationLooped)
                this->AnimatePlayerRun(gameEngine);
        }
        else
        {
            if (!this->bPlayerWalkAnimationLooped)
                this->AnimatePlayerWalk(gameEngine);
        }
    }

    else if (!this->bPlayerWalkAnimationLooped && !this->bPlayerIdleAnimationLooped && !this->bPlayerRunAnimationLooped) // Default is Idle Animation
    {
        this->mPlayerVelocity = glm::vec3(0.f); // Resetting player velocity when none of the movement keys are pressed

        this->AnimatePlayerIdle(gameEngine);
    }
}

void sPlayerAttributes::UpdatePlayerPosition(cControlGameEngine& gameEngine)
{
    //-----------------------------------Update player position---------------------------------------------------------

    this->mPlayerPosition += this->mPlayerVelocity * static_cast <float> (gameEngine.deltaTime);

    //-----------------------------------Update player rotation---------------------------------------------------------

    glm::vec3 DirectionToLookAt = GetDirectionToLook();

    glm::quat targetRotationQuat = glm::rotation(this->mPlayerForward, DirectionToLookAt);

    glm::quat rotationQuat = gameEngine.GetModelRotationQuat(this->mPlayerModelName);

    glm::quat interpolatedQuat = glm::slerp(glm::normalize(rotationQuat), glm::normalize(targetRotationQuat), this->mPlayerRotationSpeed * static_cast <float> (gameEngine.deltaTime));

    //----------------------------------Update player mesh model--------------------------------------------------------

    gameEngine.MoveModel(this->mPlayerModelName, this->mPlayerPosition.x, this->mPlayerPosition.y, this->mPlayerPosition.z);
    gameEngine.RotateMeshModelUsingQuaternion(this->mPlayerModelName, interpolatedQuat);
}
