#pragma once

#include "pch.h"
#include "cSoftBody.h"

// Windows call
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN	

struct sSoftBodyThreadDetails
{
    //cSoftBody* softBody;

    double idealFrameRate = 0.0;

    bool bThreadRunning = false;
    bool bIsAlive = true;

    DWORD sleepTime;

    std::vector < cSoftBody* > softBodyList;
};

class cSoftBodyThreaded
{

public:
    unsigned int TOTAL_SOFT_BODY_HELD_BY_THREAD = 5;

    cSoftBodyThreaded();

    ~cSoftBodyThreaded();

    void CreateNewSoftBodyThread(cSoftBody* newSoftBody, double frameRateDesired);

    void MakeThreadsRunnable();

    void TerminateThreads();

    void CheckThreadActivity();
private:
    unsigned int TOTAL_THREAD_ID = 0;

    std::vector < sSoftBodyThreadDetails* > mSoftBodyThreadDetailsList;
    std::vector < HANDLE > mThreadHandlesList;
};