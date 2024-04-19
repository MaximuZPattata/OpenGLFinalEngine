#pragma once
#include <cControlGameEngine.h>

//enum class ePlayerDirection
//{
//	FRONT,
//	BACK,
//	LEFT,
//	RIGHT,
//	FRONT_RIGHT,
//	FRONT_LEFT,
//	BACK_RIGHT,
//	BACK_LEFT
//};

enum class ePlayerState
{
	DEFAULT_STATE,
	SHIELD_STATE,
	ITEM_PICKUP_STATE,
	CARRYING_TORCH_STATE
};

struct sPlayerAttributes
{
	std::string mPlayerModelName = "";
	std::string mRunAnimationName = "";
	std::string mWalkAnimationName = "";
	std::string mIdleAnimationName = "";

	float mPlayerYaw = 0.f;
	float mPlayerPitch = 0.f;
	float mPlayerWalkSpeed = 0.f;
	float mPlayerRunSpeed = 0.f;
	float mPlayerRotationSpeed = 0.f;

	bool bPlayerWalkAnimationLooped = false;
	bool bPlayerRunAnimationLooped = false;
	bool bPlayerIdleAnimationLooped = false;

	glm::vec3 mPlayerPosition = glm::vec3(0.f);
	glm::vec3 mPlayerVelocity = glm::vec3(0.f);
	glm::vec3 mPlayerForward = glm::vec3(0.f);
	glm::vec3 mPlayerMovingDirection = glm::vec3(0.f, 0.f, -1.f);

	std::map<int, bool> mKeyPressedStateMap;

	ePlayerState playerState = ePlayerState::DEFAULT_STATE;

	void InitializePlayer(cControlGameEngine& gameEngine, std::string playerModelName, std::string idleAnimationName, std::string walkAnimationName,
		std::string runAnimationName, float walkSpeed, float runSpeed, float rotationSpeed, float camHeight, float camDistance);
	void AnimatePlayerWalk(cControlGameEngine& gameEngine);
	void AnimatePlayerIdle(cControlGameEngine& gameEngine);
	void AnimatePlayerRun(cControlGameEngine& gameEngine);
	void UpdatePlayerState(cControlGameEngine& gameEngine);

private:
	void UpdateKeyPressedStateForPlayerControl(cControlGameEngine& gameEngine);
	void UpdatePlayerPosition(cControlGameEngine& gameEngine);

	glm::vec3 GetDirectionToLook();
};
