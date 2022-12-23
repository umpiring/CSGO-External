#pragma once
#include "memory.h"
#include "variables.h"
#include "Overlay.h"
#include "config.h"

namespace Aimbot {



	



	void AimLoop() {
		auto mem = Memory{ "csgo.exe" };
		const auto client = mem.GetModuleAddress("client.dll");
		const auto engine = mem.GetModuleAddress("engine.dll");


		while (true)
		{
			if (Config::toggleAimbot)
			{


				int vkKey[] = { 0x02, 0x01, 0x06 , 0x05, 0x10, 0x12 };

				std::this_thread::sleep_for(std::chrono::milliseconds(1));

				if (!GetAsyncKeyState(vkKey[Vars::selection]))
					continue;


				const auto localPlayer = memory.Read<std::uintptr_t>(client + Offsets::dwLocalPlayer);
				const auto localTeam = memory.Read<std::int32_t>(localPlayer + Offsets::m_iTeamNum);

				const auto localEyePosition = memory.Read<Vector3>(localPlayer + Offsets::m_vecOrigin) +
					memory.Read<Vector3>(localPlayer + Offsets::m_vecViewOffset);


				const auto clientState = memory.Read<std::uintptr_t>(engine + Offsets::dwClientState);

				const auto localPlayerId =
					memory.Read<std::int32_t>(clientState + Offsets::dwClientState_GetLocalPlayer);

				const auto viewAngles = memory.Read<Vector3>(clientState + Offsets::dwClientState_ViewAngles);
				const auto aimPunch = memory.Read<Vector3>(localPlayer + Offsets::m_aimPunchAngle) * 2;

				auto bestFov1 = 10.f; // fov
				auto bestAngle = Vector3{ };

				for (auto i = 1; i < 64; ++i)
				{
					const auto player = memory.Read<std::uintptr_t>(client + Offsets::dwEntityList + i * 0x10);

					if (memory.Read<std::int32_t>(player + Offsets::m_iTeamNum) == localTeam)
						continue;

					if (memory.Read<bool>(player + Offsets::m_bDormant))
						continue;

					if (memory.Read<std::int32_t>(player + Offsets::m_lifeState))
						continue;

					if (memory.Read<std::int32_t>(player + Offsets::m_bSpottedByMask) & (1 << localPlayerId))
					{
						const auto boneMatrix = memory.Read<std::uintptr_t>(player + Offsets::m_dwBoneMatrix);


						const auto playerHeadPosition = Vector3{
							memory.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
							memory.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
							memory.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)
						};

						const auto angle = CalculateAngle(
							localEyePosition,
							playerHeadPosition,
							viewAngles + aimPunch
						);

						const auto fov = std::hypot(angle.x, angle.y);

						if (fov < bestFov1)
						{
							bestFov1 = fov;
							bestAngle = angle;
						}
					}
				}

				if (!bestAngle.IsZero())
					memory.Write<Vector3>(clientState + Offsets::dwClientState_ViewAngles, viewAngles + bestAngle / Vars::smoothing); // smoothing

			}
		}
	}
}
