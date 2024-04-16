#pragma once

struct sMouseAttributes
{
	//-------------------Common Mouse Attributes------------------------

	float mouseSensitivity = 0.05f;

	//------------------Free Flow Cam Attributes------------------------

	float freeCamLastX = 0.0f, freeCamLastY = 0.0f;

	bool bMouseMoved = true;
	bool bEnableMouseMovement = false;

	void resetMouseMoved()
	{
		if (!bMouseMoved)
			bMouseMoved = true;
	}

	//-----------Third Person View Cam(TPV) Attributes------------------

	float scrollSpeed = 0.5f;
	float minZoomDistance = 20.f;
	float maxZoomDistance = 100.f;
	float TPV_maxPitch = 89.f;
	float TPV_maxYaw = 360.f;
	float TPV_minYaw = 0.f;

	double TPV_LastX = 0.f;
	double TPV_LastY = 0.f;
};