#pragma once
#include <iostream>
#include "Aimbot.h"

#include "variables.h"
#define FL_ONGROUND (1 << 0)

// this is idiotic lazy code NOT MEANT TO BE THE MOST FUCKING OPTIMIZED BEST CHEAT
// im too lazy to set it up so i don't have to re initialize variables 

namespace fov {


	void playerFOV()
	{
		auto mem = Memory{ "csgo.exe" };
		const auto client = mem.GetModuleAddress("client.dll");


		while (true)
		{
			if (Config::fovChanger)
			{

				auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);

				mem.Write<int>(localPlayer + Offsets::m_iFOV, Vars::playerFOV);


			}

			if (Config::bhop)
			{
				auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);

				if (localPlayer)
				{
					const auto onGround = mem.Read<bool>(localPlayer + Offsets::m_fFlags);

					if (GetAsyncKeyState(VK_SPACE))
						(onGround & (1 << 0)) ?
						mem.Write<std::uintptr_t>(client + Offsets::dwForceJump, 6) :
						mem.Write<std::uintptr_t>(client + Offsets::dwForceJump, 4);
				}

			}

			auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);

			if (Config::toggleThird)
			{

				bool isScoped = mem.Read<bool>(localPlayer + Offsets::m_bIsScoped);

				if (!isScoped)
				{
					mem.Write<int>(localPlayer + Offsets::m_iObserverMode, 1);

				}
			}
			else {
				mem.Write<int>(localPlayer + Offsets::m_iObserverMode, 0);

			}
			
		}



		
	}
}