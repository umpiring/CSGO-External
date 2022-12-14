#pragma once

#include <iostream>

#include "memory.h"

#include "variables.h"
#include "config.h"
namespace chams {
	bool toggle = false;
	float brightness = 25.f;


	static float enemcol[] = { 0.f, 1.f, 0.f };
	static float tcol[] = { 0.f, 1.f, 1.f };


	void playerChams()
	{
		auto mem = Memory{ "csgo.exe" };
		const auto client = mem.GetModuleAddress("client.dll");
		const auto engine = mem.GetModuleAddress("engine.dll");


	

		auto eColour = Colour{ enemcol[0], enemcol [1], enemcol [2] }; // enem
		auto eColo2ur = Colour{ 0.f, 1.f, 0.f }; // enem
		auto tColour = Colour{ tcol[0], tcol[1], tcol[2] }; // team




		while (true) {


			if (Config::toggleChams)
			{
				auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);
				auto tTeam = mem.Read<int32_t>(localPlayer + Offsets::m_iTeamNum);


				for (int i = 0; i < 64; i++)
				{
					auto ent = mem.Read<uintptr_t>(client + Offsets::dwEntityList + i * 0x10);
					auto getTeam = mem.Read<int32_t>(ent + Offsets::m_iTeamNum);


					if (getTeam == tTeam)
					{
						mem.Write<Colour>(ent + Offsets::m_clrRender, eColour);


					}
					else {
						mem.Write<Colour>(ent + Offsets::m_clrRender, tColour);
					}
					
					auto light = static_cast<uintptr_t>(engine + Offsets::model_ambient_min - 0x2c);
					mem.Write<int32_t>(engine + Offsets::model_ambient_min, *reinterpret_cast<uintptr_t*>(&brightness) ^ light);
				}
			}
			
		}

	}
}