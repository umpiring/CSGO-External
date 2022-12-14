#pragma once
#include "variables.h"
#include "imgui.h"
namespace Config {
	bool toggleAimbot = false;

	bool toggleChams = false;
	bool toggleOutlines = false;
	bool draw = false;
	bool toggleBoxes = false;
	bool toggleHealth = false;

	bool rbw = false;

	bool toggleThird = false;
	bool fovChanger = false;

	bool bhop = false;

	bool radarHack = false;
}

namespace Vars {
	const char* keys[] = { "RMOUSE", "LMOUSE", "MOUSE5", "MOUSE4", "SHIFT", "ALT", };
	static int selection = 0; // aim key selection
	float smoothing = 8.f; // smoothing
	int playerFOV = 100;

	static float colour[] = { 1.f,0.f,0.f,1.f };
	static float boxCol[] = { 1.f,0.f,0.f};
	static float textCol[] = { 1.f,0.f,0.f};

	float boxThick = 1.f;


	




}