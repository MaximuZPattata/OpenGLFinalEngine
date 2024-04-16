#include "pch.h"
#include "cLightManager.h"

cLight::cLight()
{
	this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->atten = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // x = constant, y = linear, z = quadratic, w = DistanceCutOff
	this->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f); // Spot, directional lights(Default is stright down)
	this->param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // x = lightType(0 = pointlight | 1 = spot light | 2 = directional light), y = inner angle, z = outer angle, w = TBD
	this->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	this->position_UL = -1;
	this->diffuse_UL = -1;
	this->specular_UL = -1;
	this->atten_UL = -1;
	this->direction_UL = -1;
	this->param1_UL = -1;
	this->param2_UL = -1;
}

void cLight::TurnOn(void)
{
	this->param2.x = 1.0f;	

	return;
}

void cLight::TurnOff(void)
{
	this->param2.x = 0.0f;

	return;
}

void cLightManager::SetUniformLocations(GLuint shaderID, int lightId)
{
	//--------------------------Position Attribute----------------------------------------------

	std::string lightAttributes = "theLights[" + std::to_string(lightId) + "].position";
	const GLchar* glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->position_UL = glGetUniformLocation(shaderID, glLightAttributes);

	//-------------------------Direction Attribute----------------------------------------------

	lightAttributes = "theLights[" + std::to_string(lightId) + "].direction";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->direction_UL = glGetUniformLocation(shaderID, glLightAttributes);

	//---------------------------Diffuse Attribute----------------------------------------------

	lightAttributes = "theLights[" + std::to_string(lightId) + "].diffuse";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->diffuse_UL = glGetUniformLocation(shaderID, glLightAttributes);

	//--------------------------Specular Attribute----------------------------------------------

	lightAttributes = "theLights[" + std::to_string(lightId) + "].specular";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->specular_UL = glGetUniformLocation(shaderID, glLightAttributes);
	
	//------------------------Attenuation Attribute---------------------------------------------

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	lightAttributes = "theLights[" + std::to_string(lightId) + "].atten";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->atten_UL = glGetUniformLocation(shaderID, glLightAttributes);
	
	//---------------------------Angle Attribute------------------------------------------------

	// x = lightType, y = inner angle, z = outer angle, w = TBD
	lightAttributes = "theLights[" + std::to_string(lightId) + "].param1";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->param1_UL = glGetUniformLocation(shaderID, glLightAttributes);
	
	//-------------------------Turned On Attribute----------------------------------------------

	lightAttributes = "theLights[" + std::to_string(lightId) + "].param2";
	glLightAttributes = lightAttributes.c_str();

	this->mLightsList[lightId]->param2_UL = glGetUniformLocation(shaderID, glLightAttributes);

	return;
}

void cLightManager::UpdateUniformValues(GLuint shaderID)
{
	for (cLight* lightSource : this->mLightsList)
	{
		glUniform4f(lightSource->position_UL, 
			lightSource->position.x, 
			lightSource->position.y, 
			lightSource->position.z, 
			lightSource->position.w);

		glUniform4f(lightSource->diffuse_UL, 
			lightSource->diffuse.x, 
			lightSource->diffuse.y, 
			lightSource->diffuse.z, 
			lightSource->diffuse.w);

		glUniform4f(lightSource->specular_UL, 
			lightSource->specular.x, 
			lightSource->specular.y, 
			lightSource->specular.z, 
			lightSource->specular.w);

		glUniform4f(lightSource->atten_UL, 
			lightSource->atten.x, 
			lightSource->atten.y, 
			lightSource->atten.z, 
			lightSource->atten.w);

		glUniform4f(lightSource->direction_UL, 
			lightSource->direction.x, 
			lightSource->direction.y, 
			lightSource->direction.z, 
			lightSource->direction.w);

		glUniform4f(lightSource->param1_UL, 
			lightSource->param1.x, 
			lightSource->param1.y, 
			lightSource->param1.z, 
			lightSource->param1.w);

		glUniform4f(lightSource->param2_UL,
			lightSource->param2.x,
			lightSource->param2.y,
			lightSource->param2.z,
			lightSource->param2.w);
	}

	return;
}

cLightManager::cLightManager()
{
}