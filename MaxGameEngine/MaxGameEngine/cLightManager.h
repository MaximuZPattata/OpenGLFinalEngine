#pragma once
#include "pch.h"

// This structure matches what's in the shader
class cLight
{
public:
    cLight();

    unsigned int lightID = 0;

    glm::vec4 position;
    glm::vec4 diffuse;	// Colour of the light (used for diffuse)
    glm::vec4 specular;	// rgb = highlight colour, w = power
    glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
    glm::vec4 direction;	// Spot, directional lights
    glm::vec4 param1;	// x = lightType(0 = pointlight | 1 = spot light | 2 = directional light), y = inner angle, z = outer angle, w = TBD 
    glm::vec4 param2;	// x = 0 for off, 1 for on

    void TurnOn(void);
    void TurnOff(void);

    // These are the uniform locations for this light in the shader
    GLint position_UL;
    GLint diffuse_UL;
    GLint specular_UL;
    GLint atten_UL;
    GLint direction_UL;
    GLint param1_UL;
    GLint param2_UL;
};

class cLightManager
{
public:
    cLightManager();

    std::vector < cLight* > mLightsList;

    // This is called once
    void SetUniformLocations(GLuint shaderID, int lightId);

    // This is called every frame
    void UpdateUniformValues(GLuint shaderID);
};

