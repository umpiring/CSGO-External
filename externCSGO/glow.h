#pragma once
#include <iostream>
#include <math.h>

#include "variables.h"
#include "config.h"
namespace glow {

	float bright = 0.00001;
	inline Vector3 RainbowESP()
	{
		static uint32_t cnt = 0;

		if (cnt++ >= (uint32_t)-1)
			cnt = 0;

		Vector3 ret = { std::sin(bright * cnt + 0) * 0.5f + 0.5f, std::sin(bright * cnt + 2) * 0.5f + 0.5f , std::sin(bright * cnt + 4) * 0.5f + 0.5f };
		return ret;
	}



	void outlineESP()
	{

		auto mem = Memory{ "csgo.exe" };
		const auto client = mem.GetModuleAddress("client.dll");

		while (true)
		{

			if (Config::toggleOutlines)
			{
				const auto glowObjectManager = mem.Read<std::uintptr_t>(client + Offsets::dwGlowObjectManager);

				for (auto i = 1; i <= 64; ++i)
				{
					const auto entity = mem.Read<std::uintptr_t>(client + Offsets::dwEntityList + i * 0x10);

					std::cout << entity << std::endl;

					const auto glowIndex = mem.Read<std::int32_t>(entity + Offsets::m_iGlowIndex);
					auto localPlayer = mem.Read<uintptr_t>(client + Offsets::dwLocalPlayer);

					auto tTeam = mem.Read<int32_t>(localPlayer + Offsets::m_iTeamNum);
					auto getTeam = mem.Read<int32_t>(entity + Offsets::m_iTeamNum);

					if (tTeam == getTeam)
						continue;

					

					if (Config::rbw)
					{
						Vars::colour[0] = RainbowESP().x;
						Vars::colour[1] = RainbowESP().y;
						Vars::colour[2] = RainbowESP().z;
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x8, Vars::colour[0]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0xC, Vars::colour[1]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x10, Vars::colour[2]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x14, Vars::colour[3]);
					}
					else {
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x8, Vars::colour[0]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0xC, Vars::colour[1]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x10, Vars::colour[2]);
						mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x14, Vars::colour[3]);
					}
					


					mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
					if (Config::rbw == true)
					{
						mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x29, false);
					}

				}
			}
		}
	}
}