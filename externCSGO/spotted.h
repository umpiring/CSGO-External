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



		while (true) {


			if (Config::radarHack)
			{
				auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);
				auto tTeam = mem.Read<int32_t>(localPlayer + Offsets::m_iTeamNum);


				for (int i = 0; i < 64; i++)
				{
					auto ent = mem.Read<uintptr_t>(client + Offsets::dwEntityList + i * 0x10);
					if (mem.Read<uintptr_t>(ent + Offsets::m_iTeamNum) == tTeam)
						continue;

					mem.Write<bool>(ent + Offsets::m_bSpotted, true);

				}
			}

		}

	}
}