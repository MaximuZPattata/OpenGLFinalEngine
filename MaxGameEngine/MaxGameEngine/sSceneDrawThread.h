#pragma once

// Windows call
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN	

struct sSceneDrawThread
{
    bool bThreadRunning = false;
    bool bIsAlive = true;

    DWORD sleepTime;

    int TotalMeshDrawmByThread = 0;
};
