#include <Windows.h>
#include <iostream>
#include "MemMan.h"

constexpr auto KeyDOWN = 0x8000;

MemMan MemClass;

struct offsets
{
	DWORD dwLocalPlayer = 0xC618AC;
	DWORD dwEntityList = 0x4C3E674;
	DWORD m_iCrosshairId = 0xB2DC;
	DWORD m_iTeamNum = 0xF0;
	DWORD m_iHealth = 0xFC;
	DWORD dwSensitivity = 0xC67114;
}	offset;

struct variables
{
	DWORD localPlayer;
	DWORD gameModule;
	int myTeam;
}	val;

void slowAim()
{
	float Sens = MemClass.readMem<float>(val.gameModule + offset.dwSensitivity);
	
	MemClass.writeMem<float>(val.gameModule + offset.dwSensitivity, (6 / Sens));
	Sleep(20);
	MemClass.writeMem<float>(val.gameModule + offset.dwSensitivity, Sens);
}

bool checkAim()
{
	int crosshair = MemClass.readMem<int>(val.localPlayer + offset.m_iCrosshairId);
	if (crosshair != 0 && crosshair < 64)
	{
		DWORD entity = MemClass.readMem<DWORD>(val.gameModule + offset.dwEntityList + ((crosshair - 1) * 0x10));
		int eTeam = MemClass.readMem<int>(entity + offset.m_iTeamNum);
		int eHealth = MemClass.readMem<int>(entity + offset.m_iHealth);
		if (eTeam != val.myTeam && eHealth > 0)
			return true;
		else
			return false;
	}
	else
		return false;
}

void handleAim()
{
	if (checkAim())
		slowAim();
}

int main()
{
	bool toggleSlow = false;
	int proc = MemClass.getProcess("csgo.exe");
	val.gameModule = MemClass.getModule(proc, "client_panorama.dll");
	val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + offset.dwLocalPlayer);

	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL)
			val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + offset.dwLocalPlayer);

	while (true)
	{
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			val.myTeam = MemClass.readMem<int>(val.localPlayer + offset.m_iTeamNum);
			toggleSlow = !toggleSlow;
		}

		if (toggleSlow)
			handleAim();

		Sleep(1);
	}
	return 0;
}