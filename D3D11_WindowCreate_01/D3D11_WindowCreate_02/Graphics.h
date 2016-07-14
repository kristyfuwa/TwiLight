#pragma once
#include <windows.h>

const bool Full_Screen = false;
const bool Vsync_Enabled = true;
const float Screen_Depth = 1000.0f;
const float Screen_Near = 0.1f;


class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initilize(int ,int ,HWND);
	void ShutDown();
	bool Frame();
private:
	bool Render();


};