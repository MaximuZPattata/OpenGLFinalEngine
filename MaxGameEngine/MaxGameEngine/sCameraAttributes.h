#pragma once
#include "pch.h"

struct sCameraAttributes
{
    float mYaw = -90.0f;   // Vertical axis(Left and Right)
    float mPitch = 0.0f;   // Horizontal axis(Up and Down)
    float mCameraHeight = 0.f;
    float mCameraDistance = 0.f;

    bool bFreeFlowCamera = false;
    bool bEditorModeOn = false;
    bool bLightEditorMode = false;

    glm::vec3 mCameraEye = glm::vec3(0.0, 0.0f, 0.0f);
    glm::vec3 mCameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 mCamUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
};