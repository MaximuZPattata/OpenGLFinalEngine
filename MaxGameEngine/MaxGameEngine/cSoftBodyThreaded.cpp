#include "pch.h"
#include "cSoftBodyThreaded.h"

CRITICAL_SECTION updateSoftBodyCriticalSection;
CRITICAL_SECTION flagCriticalSection;

cSoftBodyThreaded::cSoftBodyThreaded() 
{
    InitializeCriticalSection(&updateSoftBodyCriticalSection);
    InitializeCriticalSection(&flagCriticalSection);
}

cSoftBodyThreaded::~cSoftBodyThreaded() 
{
    DeleteCriticalSection(&updateSoftBodyCriticalSection);
    DeleteCriticalSection(&flagCriticalSection);
}

DWORD WINAPI UpdateSoftBodyThread(LPVOID lpParameter)
{
    //---------------------Initialize variables----------------------------

    sSoftBodyThreadDetails* threadedSoftBody = (sSoftBodyThreadDetails*)lpParameter;

    double lastFrameTime = glfwGetTime();
    double currentTime = 0.f; 
    double deltaTime = 0.f;
    double totalElapsedFrameTime = 0.f;

    DWORD sleepTimeMilliSec = 1;

    while (threadedSoftBody->bIsAlive)
    {
        if (threadedSoftBody->bThreadRunning)
        {
            //--------------Check total time elapsed-----------------------

            currentTime = glfwGetTime();
            deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            totalElapsedFrameTime += deltaTime;

            //-------Compare elapsed time with ideal frame rate------------

            if (totalElapsedFrameTime >= threadedSoftBody->idealFrameRate)
            {
                totalElapsedFrameTime = 0.0;

                EnterCriticalSection(&updateSoftBodyCriticalSection);

                for (cSoftBody* currentSoftBody : threadedSoftBody->softBodyList)
                {
                    currentSoftBody->VerletUpdate(threadedSoftBody->idealFrameRate);
                    currentSoftBody->ApplyCollision(threadedSoftBody->idealFrameRate, threadedSoftBody->softBodyList);
                    currentSoftBody->SatisfyConstraints();
                    currentSoftBody->UpdateVertexPositions();
                    currentSoftBody->UpdateNormals();
                }

                LeaveCriticalSection(&updateSoftBodyCriticalSection);
            }

            //threadedSoftBody->bThreadRunning = false;

            Sleep(sleepTimeMilliSec);
        }
    }

    return 0;
}

void cSoftBodyThreaded::CreateNewSoftBodyThread(cSoftBody* newSoftBody, double frameRateDesired)
{
    sSoftBodyThreadDetails* newSoftBodyThreadDetail = NULL;

    //---Check if softBody thread already has particular amount of soft bodies in it's list---

    for (int count = 0; count < mSoftBodyThreadDetailsList.size(); count++)
    {
        if (mSoftBodyThreadDetailsList[count]->softBodyList.size() == TOTAL_SOFT_BODY_HELD_BY_THREAD)
            continue;
        else
            newSoftBodyThreadDetail = mSoftBodyThreadDetailsList[count];
    }

    //----------Create new struct instanct for soft body thread details-----------------------
    
    if (newSoftBodyThreadDetail == NULL)
    {
        newSoftBodyThreadDetail = new sSoftBodyThreadDetails();

        //------------Create new thread connecting the new struct instance------------------------

        DWORD ThreadId = TOTAL_THREAD_ID;
        HANDLE threadHandle = 0;

        TOTAL_THREAD_ID++;

        threadHandle = CreateThread(NULL, 0, ::UpdateSoftBodyThread, newSoftBodyThreadDetail, 0, &ThreadId);

        if (threadHandle == NULL)
            std::cout << "ERROR: FAILED TO CREATE THREAD" << std::endl;
        else
        {
            std::cout << "THREAD CREATED SUCCESSFULLY" << std::endl;

            mThreadHandlesList.push_back(threadHandle);
        }
    }

    newSoftBodyThreadDetail->softBodyList.push_back(newSoftBody);
    newSoftBodyThreadDetail->idealFrameRate = frameRateDesired;
    newSoftBodyThreadDetail->bThreadRunning = false;

    mSoftBodyThreadDetailsList.push_back(newSoftBodyThreadDetail);
}

void cSoftBodyThreaded::MakeThreadsRunnable()
{
    for (sSoftBodyThreadDetails* softBodyThread : mSoftBodyThreadDetailsList)
        softBodyThread->bThreadRunning = true;
}

void cSoftBodyThreaded::TerminateThreads()
{
    if (!mThreadHandlesList.empty())
    {
        std::cout << std::endl;

        for (int index = 0; index < mThreadHandlesList.size(); index++)
        {
            //WaitForSingleObject(mThreadHandlesList[index], INFINITE);

            if (CloseHandle(mThreadHandlesList[index]))
                std::cout << "THREAD EXIT SUCCESSFUL" << std::endl;

            else
            {
                DWORD dwError = GetLastError();

                std::cout << "ERROR : UNABLE TO EXIT THREAD - [" << dwError << "]" << std::endl;
            }
        }

        std::cout << std::endl;
    }
}

void cSoftBodyThreaded::CheckThreadActivity()
{
    EnterCriticalSection(&flagCriticalSection);

    for (sSoftBodyThreadDetails* softBodyThread : mSoftBodyThreadDetailsList)
    {
        int softBodyInMotionCount = 0;

        for (int index = 0; index < softBodyThread->softBodyList.size(); index++)
        {
            if (!softBodyThread->softBodyList[index]->bSoftBodyInMotion)
                softBodyInMotionCount++;
        }

        if (softBodyInMotionCount == softBodyThread->softBodyList.size())
        {
            softBodyThread->bIsAlive = false;
            softBodyThread->bThreadRunning = false;
        }
      
       /*if (!softBodyThread->softBody->bSoftBodyInMotion)
        {
            softBodyThread->bIsAlive = false;
            softBodyThread->bThreadRunning = false;
        }*/

    }

    LeaveCriticalSection(&flagCriticalSection);
}

