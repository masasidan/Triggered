#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include "MemMan.h"

using namespace std;

MemMan MemClass;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      // Black
#define RED     "\033[31m"      // Red
#define GREEN   "\033[32m"      // Green
#define YELLOW  "\033[33m"      // Yellow
#define BLUE    "\033[34m"      // Blue
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan
#define WHITE   "\033[37m"      // White
#define BOLDBLACK   "\033[1m\033[30m"      // Bold Black
#define BOLDRED     "\033[1m\033[31m"      // Bold Red
#define BOLDGREEN   "\033[1m\033[32m"      // Bold Green
#define BOLDYELLOW  "\033[1m\033[33m"      // Bold Yellow
#define BOLDBLUE    "\033[1m\033[34m"      // Bold Blue
#define BOLDMAGENTA "\033[1m\033[35m"      // Bold Magenta
#define BOLDCYAN    "\033[1m\033[36m"      // Bold Cyan
#define BOLDWHITE   "\033[1m\033[37m"      // Bold White
#define HEADER   "\033[95m"

bool Triggerbot = false;
bool GlowESP = false;
bool BHop = false;

string Triggerbot_status = "\033[31m[OFF]";
string GlowESP_status = "\033[31m[OFF]";
string Bunnyhop_status = "\033[31m[OFF]";

struct offsets
{
	DWORD dwLocalPlayer = 0xDA244C; // MAY BE OUTDATED
	DWORD dwEntityList = 0x4DBC5B4; // MAY BE OUTDATED
	DWORD dwForceAttack = 0x31ECB34; // MAY BE OUTDATED
	DWORD dwGlowObjectManager = 0x5304AC0; // MAY BE OUTDATED
	DWORD dwRadarBase = 0x51F132C; // MAY BE OUTDATED
	DWORD ForceJump = 0x52663C4; // MAY BE OUTDATED
	DWORD m_iGlowIndex = 0x10488; // MAY BE OUTDATED
	DWORD m_iTeamNum = 0xF4; // MAY BE OUTDATED
	DWORD m_iCrosshairId = 0x11438; // MAY BE OUTDATED
	DWORD m_fFlags = 0x104; // MAY BE OUTDATED
} offset;

struct variables
{
	DWORD clientModule;
	DWORD engineModule;

	float version = 1.0; // PROGRAM VERSION
} val;

void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

void refreshState(string state) {
	clear();

	if (state == "riggerbot") {
		Triggerbot_status = "\033[32m[ON]";
	}
	else if (state == "lowESP") {
		GlowESP_status = "\033[32m[ON]";
	}
	else if (state == "HOP") {
		Bunnyhop_status = "\033[32m[ON]";
	}
	else if (state == "Triggerbot") {
		Triggerbot_status = "\033[31m[OFF]";
	}
	else if (state == "GlowESP") {
		GlowESP_status = "\033[31m[OFF]";
	}
	else if (state == "BHOP") {
		Bunnyhop_status = "\033[31m[OFF]";
	}

	cout << HEADER << "====================================" << endl;
	cout << HEADER << "           Triggered v" << val.version << endl;
	cout << HEADER << "====================================\n" << endl;

	cout << BOLDYELLOW << "Triggerbot "<< Triggerbot_status << " " << BOLDBLUE << "[DEL]" << endl;
	cout << BOLDYELLOW << "GlowESP " << GlowESP_status << " " << BOLDBLUE << "[HOME]" << endl;
	cout << BOLDYELLOW << "Bunnyhop " << Bunnyhop_status << " " << BOLDBLUE << "[INS]\n" << endl;
}

int main()
{
	system("CLS");
	SetConsoleTitleA("Triggered / Made by idan#1000");

	int procID = MemClass.getProcess("csgo.exe");

	val.clientModule = MemClass.getModule(procID, "client.dll");
	val.engineModule = MemClass.getModule(procID, "engine.dll"); 

	cout << HEADER << "====================================" << endl;
	cout << HEADER << "           Triggered v" << val.version << endl;
	cout << HEADER << "====================================\n" << endl;

	cout << BOLDYELLOW << "Triggerbot " << RED << "[OFF] " << BOLDBLUE << "[DEL]" << endl;
	cout << BOLDYELLOW << "GlowESP " << RED << "[OFF] " << BOLDBLUE << "[HOME]" << endl;
	cout << BOLDYELLOW << "Bunnyhop " << RED << "[OFF] " << BOLDBLUE << "[INS]\n" << endl;

	while (true)
	{
		//BUNNYHOP KEY
		if (GetAsyncKeyState(VK_INSERT)) {
			Sleep(100);
			BHop = !BHop;
			refreshState("BHOP" + BHop);
		}
		
		//TRIGGERBOT KEY
		if (GetAsyncKeyState(VK_DELETE)) {
			Sleep(100);
			Triggerbot = !Triggerbot;
			refreshState("Triggerbot" + Triggerbot);
		}

		//GLOWESP KEY
		if (GetAsyncKeyState(VK_HOME)) {
			Sleep(100);
			GlowESP = !GlowESP;
			refreshState("GlowESP" + GlowESP);
		}

		//BUNNYHOP MODULE
		if (GetAsyncKeyState(VK_SPACE) && BHop) {
			DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
			DWORD onGround = MemClass.readMem<DWORD>(player + offset.m_fFlags);

			if (player && onGround == 257) {
				MemClass.writeMem<int>(val.clientModule + offset.ForceJump, 5);
				Sleep(10);
				MemClass.writeMem<int>(val.clientModule + offset.ForceJump, 4);
			}
		}

		//GLOWESP MODULE
		if (GlowESP) {
			DWORD glow_manager = MemClass.readMem<DWORD>(val.clientModule + offset.dwGlowObjectManager);

			for (int x = 1; x < 32; ++x) {
				DWORD entity = MemClass.readMem<DWORD>(val.clientModule + offset.dwEntityList + x * 0x10);
				if (entity) {
					DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
					DWORD entity_team_id = MemClass.readMem<DWORD>(entity + offset.m_iTeamNum);
					DWORD entity_glow = MemClass.readMem<DWORD>(entity + offset.m_iGlowIndex);
					DWORD localTeam = MemClass.readMem<DWORD>(player + offset.m_iTeamNum);

					if (entity_team_id != localTeam) {
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x8, 1.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x12, 0.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0xC, 0.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x14, 1.0f);
						MemClass.writeMem<DWORD>(glow_manager + entity_glow * 0x38 + 0x28, 1);
					}

					if (entity_team_id == localTeam) {
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x8, 0.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x12, 0.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0xC, 1.0f);
						MemClass.writeMem<float>(glow_manager + entity_glow * 0x38 + 0x14, 1.0f);
						MemClass.writeMem<DWORD>(glow_manager + entity_glow * 0x38 + 0x28, 1);
					}
				}
			}
		}

		//TRIGGERBOT MODULE
		if (Triggerbot) {
			DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
			DWORD inCross = MemClass.readMem<DWORD>(player + offset.m_iCrosshairId);
			DWORD getTeam = MemClass.readMem<DWORD>(val.clientModule + offset.dwEntityList + ((inCross - 1) * 0x10));
			DWORD myTeam = MemClass.readMem<DWORD>(player + offset.m_iTeamNum);
			DWORD crosshairTeam = MemClass.readMem<DWORD>(getTeam + offset.m_iTeamNum);
			DWORD enemyHealth = MemClass.readMem<DWORD>(getTeam + offset.m_iTeamNum);


			if (inCross > 0 && inCross < 64 && crosshairTeam != myTeam) {
				MemClass.writeMem<int>(val.clientModule + offset.dwForceAttack, 6);
			}
		}

		//EXIT PROGRAM
		if (GetAsyncKeyState(VK_END)) {
			refreshState("BHOP");
			refreshState("Triggerbot");
			refreshState("GlowESP");
			break;
		}
	}

	cout << "Shutting down.." << endl;
	Sleep(1000);

	return 0;
}
