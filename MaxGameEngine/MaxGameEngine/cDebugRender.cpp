#include "cDebugRender.h"

cDebugRenderer::cDebugRenderer()
{
    this->mSizeOfLineVertexBuffer = 0;
    this->mLinesVerticesToDraw = NULL;
}

float getRandFloat(float a, float b) 
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

bool cDebugRenderer::Initialize(cShaderManager* mDebugRendererShaderManager, cVAOManager* mDebugRendererMeshManager, GLuint& cDebugRenderer_vertex_buffer, GLuint& cDebugRenderer_VAO)
{
    mDebugRendererShaderManager = new cShaderManager();

    mDebugRendererShaderManager->setBasePath("assets/shaders");

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "debugObject_vertexShader.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "debugObject_fragmentShader01.glsl";

    //if (!mDebugRendererShaderManager->createProgramFromFile("debugObjectShader", vertexShader, fragmentShader))
    //{
    //    this->mLastError = "ERROR: " + mDebugRendererShaderManager->getLastError();
    //    return false;
    //}

    //GLuint debugShaderProgramID = mDebugRendererShaderManager->getIDFromFriendlyName("debugObjectShader");

    ////----------------Allocating CPU side array to hold our lines--------------------------------
    //
    //this->mLinesVerticesToDraw = new sLineVertexBuffer[LINE_VERTEX_BUFFER_SIZE];
   
    ////---------------------------Clearing it to zero---------------------------------------------

    //memset(this->mLinesVerticesToDraw, 0, LINE_VERTEX_BUFFER_SIZE * sizeof(sLineVertexBuffer));

    //this->mSizeOfLineVertexBuffer = 0;

    ////-------------------------------Adding Lines------------------------------------------------

    //const float MAX_LINE_POSITION = 1000.0f;

    //for (int count = 0; count != 500; count++)
    //{
    //    glm::vec3 lineStart = glm::vec3(getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION), getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
    //        getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION));

    //    glm::vec3 lineEnd = glm::vec3(getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION), getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION),
    //        getRandFloat(-MAX_LINE_POSITION, MAX_LINE_POSITION));

    //    glm::vec4 lineColour = glm::vec4(getRandFloat(0.0f, 1.0f), getRandFloat(0.0f, 1.0f), getRandFloat(0.0f, 1.0f), 1.0f);

    //    this->AddLine(lineStart, lineEnd, lineColour);
    //}

    ////------------------------------Binding Buffer-----------------------------------------------

    //// Copies the data from the C++/CPU side to the GPU, overwriting everything
    //unsigned int numberOfBytesToCopy = sizeof(sLineVertexBuffer) * LINE_VERTEX_BUFFER_SIZE;

    //glGenVertexArrays(1, &cDebugRenderer_VAO);
    //
    //glBindVertexArray(cDebugRenderer_VAO);

    //glGenBuffers(1, &cDebugRenderer_vertex_buffer);

    //glBindBuffer(GL_ARRAY_BUFFER, cDebugRenderer_vertex_buffer);

    //glBufferData(GL_ARRAY_BUFFER, sizeof(numberOfBytesToCopy), this->mLinesVerticesToDraw, GL_DYNAMIC_DRAW);

    //GLint vPosition_location = glGetAttribLocation(debugShaderProgramID, "vPosition");
    //GLint vColour_location = glGetAttribLocation(debugShaderProgramID, "vColour");

    //glEnableVertexAttribArray(vPosition_location);
    //glVertexAttribPointer(vPosition_location, 4, GL_FLOAT, GL_FALSE, sizeof(sLineVertexBuffer), (void*)offsetof(sLineVertexBuffer, x)); 

    //glEnableVertexAttribArray(vColour_location);
    //glVertexAttribPointer(vColour_location, 4, GL_FLOAT, GL_FALSE, sizeof(sLineVertexBuffer), (void*)offsetof(sLineVertexBuffer, r)); 

    //glBindVertexArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glDisableVertexAttribArray(vPosition_location);
    //glDisableVertexAttribArray(vColour_location);

    ////----------------------------Loading the models---------------------------------------------

    //mDebugRendererMeshManager = new cVAOManager();
    //mDebugRendererMeshManager->setBasePath("assets/models");

    //sModelDrawInfo sphereMeshInfo;
    //sModelDrawInfo cubeMeshInfo;

    //std::string sphereModelName = "Debug_Sphere";
    //std::string cubeModelName = "Debug_Cube";

    //mDebugRendererMeshManager->LoadModelIntoVAO(sphereModelName, "Sphere_1_unit_Radius.ply", sphereMeshInfo, debugShaderProgramID);
    //mDebugRendererMeshManager->LoadModelIntoVAO(cubeModelName, "Cube_1x1x1_xyz_n_rgba.ply", cubeMeshInfo, debugShaderProgramID);

    return true;
}

void cDebugRenderer::CreateNewDebugSpheres(cMesh* newSphereModel, std::vector <glm::vec3> positionsList)
{
    sDebugSphereDetails* newDebugSphere = new sDebugSphereDetails();

    newDebugSphere->sphereModel = newSphereModel;
    newDebugSphere->spherePositionsList = positionsList;

    mDebugSphereDetailsList.push_back(newDebugSphere);
}

void cDebugRenderer::UpdateDebugSpherePositions(std::string debugSphereName, std::vector <glm::vec3> positionsList)
{
    //------------------Finding the right sDebugSphereDetails struct instance-----------------------

    for (int index = 0; index < mDebugSphereDetailsList.size(); index++)
    {
        if (mDebugSphereDetailsList[index]->sphereModel->friendlyName == debugSphereName)
        {
            mDebugSphereDetailsList[index]->spherePositionsList.clear();
            mDebugSphereDetailsList[index]->spherePositionsList = positionsList;
            
            return;
        }
    }

    std::cout << "ERROR : COULDN'T FIND DEBUG SPHERE MODEL OF THAT NAME" << std::endl;
}

std::vector<glm::vec3> cDebugRenderer::GetSphereModelPositions(std::string debugSphereMeshName)
{
    //------------------Finding the right sDebugSphereDetails struct instance-----------------------

    for (int index = 0; index < mDebugSphereDetailsList.size(); index++)
    {
        if (mDebugSphereDetailsList[index]->sphereModel->friendlyName == debugSphereMeshName)
            return mDebugSphereDetailsList[index]->spherePositionsList;
    }

    std::cout<<"ERROR : COULDN'T FIND DEBUG SPHERE MODEL OF THAT NAME"<<std::endl;

    std::vector<glm::vec3> emptyVector;

    return emptyVector;
}


//std::string cDebugRenderer::getLastError(void)
//{
//    std::string theLastError = this->mLastError;
//    this->mLastError = "";
//
//    return theLastError;
//}
//
//void cDebugRenderer::ShutDown(cShaderManager* mDebugRendererShaderManager)
//{
//    delete mDebugRendererShaderManager;
//}
//
//void cDebugRenderer::AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA)
//{
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].x = startXYZ.x;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].y = startXYZ.y;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].z = startXYZ.z;
//
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].r = RGBA.r;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].g = RGBA.g;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].b = RGBA.b;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 0].a = RGBA.a;
//
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].x = endXYZ.x;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].y = endXYZ.y;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].z = endXYZ.z;
//
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].r = RGBA.r;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].g = RGBA.g;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].b = RGBA.b;
//    this->mLinesVerticesToDraw[this->mSizeOfLineVertexBuffer + 1].a = RGBA.a;
//
//    this->mSizeOfLineVertexBuffer += 2;
//}
//
//void cDebugRenderer::RenderDebugObjects(double deltaTime, glm::mat4 matView, glm::mat4 matProjection, cShaderManager* mDebugRendererShaderManager, GLuint& cDebugRenderer_VAO)
//{
//    GLuint shaderID = mDebugRendererShaderManager->getIDFromFriendlyName("debugObjectShader");
//
//    // Copies the data from the C++/CPU side to the GPU, overwriting whatever is there
//    unsigned int numberOfBytesToCopy = sizeof(sLineVertexBuffer) * this->mSizeOfLineVertexBuffer;
//
//    glUseProgram(shaderID);
//
//    GLint mvp_location = glGetUniformLocation(shaderID, "MVP");
//
//    glm::mat4 matModel = glm::mat4(1.0f);
//    glm::mat4 matMVP = matProjection * matView * matModel;
//
//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(matMVP));
//
//    glBindVertexArray(cDebugRenderer_VAO);
//
//    glDrawArrays(GL_LINES, 0, this->mSizeOfLineVertexBuffer);
//
//    glBindVertexArray(0);
//}
