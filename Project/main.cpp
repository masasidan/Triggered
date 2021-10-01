#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include "MemMan.h"
//#include <d3d9.h>
//#include <d3dx9.h>

//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")

using namespace std;

MemMan MemClass;

//typedef interface ID3DXFont ID3DXFont;
//typedef interface ID3DXFont* LPD3DXFONT;

typedef DWORD D3DCOLOR;

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

#define EnemyPen 0x000000FF
#define TeammatePen 0x4DFF00

HBRUSH EnemyBrush = CreateSolidBrush(0x000000FF);
HBRUSH TeammateBrush = CreateSolidBrush(0x4DFF00);

HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
HDC hdc = GetDC(hwnd);

RECT rect;

int rectt = GetClientRect(hwnd, &rect);

int screenX = rect.right - rect.left;
int screenY = rect.bottom - rect.top;

const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN); const int xhairx = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN); const int xhairy = SCREEN_HEIGHT / 2;

int closest;

bool Triggerbot = false;
bool GlowESP = false;
bool ESP = false;
bool BHop = false;
bool Aimbot = false;

string Triggerbot_status = "\033[31m[OFF]";
string GlowESP_status = "\033[31m[OFF]";
string ESP_status = "\033[31m[OFF]";
string Aimbot_status = "\033[31m[OFF]";
string Bunnyhop_status = "\033[31m[OFF]";

struct offsets
{
	DWORD dwLocalPlayer = 0xDA244C; // MAY BE OUTDATED
	DWORD dwEntityList = 0x4DBC5B4; // MAY BE OUTDATED
	DWORD dwViewMatrix = 0x4DADEB4; // MAY BE OUTDATED
	DWORD dwForceAttack = 0x31ECB34; // MAY BE OUTDATED
	DWORD dwGlowObjectManager = 0x5304AC0; // MAY BE OUTDATED
	DWORD dwRadarBase = 0x51F132C; // MAY BE OUTDATED
	DWORD dwClientState = 0x588FEC; // MAY BE OUTDATED
	DWORD dwClientState_PlayerInfo = 0x52C0; // MAY BE OUTDATED
	DWORD ForceJump = 0x52663C4; // MAY BE OUTDATED
	DWORD m_dwBoneMatrix = 0x26A8; // MAY BE OUTDATED
	DWORD m_bDormant = 0xED; // MAY BE OUTDATED
	DWORD m_iGlowIndex = 0x10488; // MAY BE OUTDATED
	DWORD m_iTeamNum = 0xF4; // MAY BE OUTDATED
	DWORD m_iHealth = 0x100; // MAY BE OUTDATED
	DWORD m_vecOrigin = 0x138; // MAY BE OUTDATED
	DWORD m_vecViewOffset = 0x108; // MAY BE OUTDATED
	DWORD m_iCrosshairId = 0x11438; // MAY BE OUTDATED
	DWORD m_fFlags = 0x104; // MAY BE OUTDATED
} offset;

struct variables
{
	DWORD clientModule;
	DWORD engineModule;

	float version = 2.0; // PROGRAM VERSION
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
	else if (state == "SP") {
		ESP_status = "\033[32m[ON]";
	}
	else if (state == "imbot") {
		Aimbot_status = "\033[32m[ON]";
	}
	else if (state == "Triggerbot") {
		Triggerbot_status = "\033[31m[OFF]";
	}
	else if (state == "GlowESP") {
		GlowESP_status = "\033[31m[OFF]";
	}
	else if (state == "ESP") {
		ESP_status = "\033[31m[OFF]";
	}
	else if (state == "Aimbot") {
		Aimbot_status = "\033[31m[OFF]";
	}
	else if (state == "BHOP") {
		Bunnyhop_status = "\033[31m[OFF]";
	}

	cout << WHITE << "* * * * * * * * * * * * * * * * * * *" << endl;
	cout << YELLOW << " _____     _                       _ " << endl;
	cout << RED << "|_   _|___|_|___ ___ ___ ___ ___ _| |" << endl;
	cout << BLUE << "  | | |  _| | . | . | -_|  _| -_| . |" << endl;
	cout << GREEN << "  |_| |_| |_|_  |_  |___|_| |___|___|" << endl;
	cout << RED << "            |___|___|v" << val.version << endl;
	cout << WHITE << "\n* * * * * * * * * * * * * * * * * * *\n" << endl;


	cout << BOLDYELLOW << "Triggerbot "<< Triggerbot_status << " " << BOLDBLUE << "[DEL]" << endl;
	cout << BOLDYELLOW << "GlowESP " << GlowESP_status << " " << BOLDBLUE << "[HOME]" << endl;
	cout << BOLDYELLOW << "ESP " << ESP_status << " " << BOLDBLUE << "[F7]" << endl;
	cout << BOLDYELLOW << "Aimbot " << Aimbot_status << " " << BOLDBLUE << "[F6]" << endl;
	cout << BOLDYELLOW << "Bunnyhop " << Bunnyhop_status << " " << BOLDBLUE << "[INS]\n" << endl;
}

struct view_matrix_tt {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector33
{
	float x, y, z;
};

class Vector3 {
public:
	float x, y, z;
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

Vector33 WorldToScreenESP(const Vector3 pos, view_matrix_tt matrix) {
	float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	float inv_w = 1.f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = screenX * .5f;
	float y = screenY * .5f;

	x += 0.5f * _x * screenX + 0.5f;
	y -= 0.5f * _y * screenY + 0.5f;

	return { x,y,w };
}

void DrawFilledRect(int x, int y, int w, int h, string player)
{
	RECT rect = { x, y, x + w, y + h };

	if (player == "enemy") {
		FillRect(hdc, &rect, EnemyBrush);
	}
	else {
		FillRect(hdc, &rect, TeammateBrush);
	}
}

void DrawBorderBox(int x, int y, int w, int h, int thickness, string player)
{
	DrawFilledRect(x, y, w, thickness, player); //Top horiz line
	DrawFilledRect(x, y, thickness, h, player); //Left vertical line
	DrawFilledRect((x + w), y, thickness, h, player); //right vertical line
	DrawFilledRect(x, y + h, w + thickness, thickness, player); //bottom horiz line
}

void DrawLine(float StartX, float StartY, float EndX, float EndY, string player)
{
	int a, b = 0;
	HPEN hOPen;
	HPEN hNPen;
	if (player == "enemy") {
		hNPen = CreatePen(PS_SOLID, 3, EnemyPen);// penstyle, width, color
	}
	else {
		hNPen = CreatePen(PS_SOLID, 3, TeammatePen);// penstyle, width, color
	}
	hOPen = (HPEN)SelectObject(hdc, hNPen);
	MoveToEx(hdc, StartX, StartY, NULL); //start
	a = LineTo(hdc, EndX, EndY); //end
	DeleteObject(SelectObject(hdc, hOPen));
}

int getTeam(uintptr_t player) {
	return MemClass.readMem<int>(player + offset.m_iTeamNum);
}

uintptr_t GetLocalPlayer() {
	return MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
}

uintptr_t GetPlayer(int index) {  //Each player has an index. 1-64
	return MemClass.readMem< uintptr_t>(val.clientModule + offset.dwEntityList + index * 0x10); //We multiply the index by 0x10 to select the player we want in the entity list.
}

int GetPlayerHealth(uintptr_t player) {
	return MemClass.readMem<int>(player + offset.m_iHealth);
}

Vector3 PlayerLocation(uintptr_t player) { //Stores XYZ coordinates in a Vector3.
	return MemClass.readMem<Vector3>(player + offset.m_vecOrigin);
}

bool DormantCheck(uintptr_t player) {
	return MemClass.readMem<int>(player + offset.m_bDormant);
}

Vector3 get_head(uintptr_t player) {
	struct boneMatrix_t {
		byte pad3[12];
		float x;
		byte pad1[12];
		float y;
		byte pad2[12];
		float z;
	};
	uintptr_t boneBase = MemClass.readMem<uintptr_t>(player + offset.m_dwBoneMatrix);
	boneMatrix_t boneMatrix = MemClass.readMem<boneMatrix_t>(boneBase + (sizeof(boneMatrix) * 8 /*8 is the boneid for head*/));
	return Vector3(boneMatrix.x, boneMatrix.y, boneMatrix.z);
}

struct view_matrix_t {
	float matrix[16];
} vm;

struct Vector3 WorldToScreen(const struct Vector3 pos, struct view_matrix_t matrix) { //This turns 3D coordinates (ex: XYZ) int 2D coordinates (ex: XY).
	struct Vector3 out;
	float _x = matrix.matrix[0] * pos.x + matrix.matrix[1] * pos.y + matrix.matrix[2] * pos.z + matrix.matrix[3];
	float _y = matrix.matrix[4] * pos.x + matrix.matrix[5] * pos.y + matrix.matrix[6] * pos.z + matrix.matrix[7];
	out.z = matrix.matrix[12] * pos.x + matrix.matrix[13] * pos.y + matrix.matrix[14] * pos.z + matrix.matrix[15];

	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = SCREEN_WIDTH * .5f;
	out.y = SCREEN_HEIGHT * .5f;

	out.x += 0.5f * _x * SCREEN_WIDTH + 0.5f;
	out.y -= 0.5f * _y * SCREEN_HEIGHT + 0.5f;

	return out;
}

float pythag(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int FindClosestEnemy() {
	float Finish;
	int ClosestEntity = 1;
	Vector3 Calc = { 0, 0, 0 };
	float Closest = FLT_MAX;
	int localTeam = getTeam(GetLocalPlayer());
	for (int i = 1; i < 64; ++i) {
		DWORD Entity = GetPlayer(i);
		int EnmTeam = getTeam(Entity); if (EnmTeam == localTeam) continue;
		int EnmHealth = GetPlayerHealth(Entity); if (EnmHealth < 1 || EnmHealth > 100) continue;
		int Dormant = DormantCheck(Entity); if (Dormant) continue;
		Vector3 headBone = WorldToScreen(get_head(Entity), vm);
		Finish = pythag(headBone.x, headBone.y, xhairx, xhairy);
		if (Finish < Closest) {
			Closest = Finish;
			ClosestEntity = i;
		}
	}

	return ClosestEntity;
}

char* getName(int index) // this returns player's name (and even more data)
{
	struct player_info
	{
		__int64 unknown;            //0x0000 
		union
		{
			__int64 teamID64;          //0x0008 - SteamID64
			struct
			{
				__int32 xuid_low;
				__int32 xuid_high;
			};
		};
		char szName[128];        //0x0010 - Player Name
		int userId;             //0x0090 - Unique Server Identifier
		char szSteamID[20];      //0x0094 - STEAM_X:Y:Z
		char pad_0x00A8[0x10];   //0x00A8
		unsigned long iSteamID;           //0x00B8 - SteamID 
		char szFriendsName[128];
		bool fakeplayer;
		bool ishltv;
		unsigned int customfiles[4];
		unsigned char filesdownloaded;
	} var;

	DWORD clientState = MemClass.readMem<DWORD>(val.engineModule + offset.dwClientState);
	DWORD userInfoTable = MemClass.readMem<DWORD>(clientState + offset.dwClientState_PlayerInfo);
	DWORD items = MemClass.readMem<DWORD>(MemClass.readMem<DWORD>(userInfoTable + 0x40) + 0xC);

	var = MemClass.readMem<player_info>(MemClass.readMem<DWORD>(items + 0x28 + ((index - 1) * 0x34)));

	string name = var.szName;

	if (name.length() < 128) {
		cout << var.szName << endl;
	}
	
	return var.szName;
}

void FindClosestEnemyThread() {
	while (true) {
		while (Aimbot) {
			closest = FindClosestEnemy();
		}
	}
}

void TriggerbotThread() {
	while (true) {
		while (Triggerbot) {
			DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
			DWORD inCross = MemClass.readMem<DWORD>(player + offset.m_iCrosshairId);
			DWORD getTeam = MemClass.readMem<DWORD>(val.clientModule + offset.dwEntityList + ((inCross - 1) * 0x10));
			DWORD myTeam = MemClass.readMem<DWORD>(player + offset.m_iTeamNum);
			DWORD crosshairTeam = MemClass.readMem<DWORD>(getTeam + offset.m_iTeamNum);
			DWORD enemyHealth = MemClass.readMem<DWORD>(getTeam + offset.m_iTeamNum);


			if (inCross > 0 && inCross < 64 && crosshairTeam != myTeam) {
				MemClass.writeMem<int>(val.clientModule + offset.dwForceAttack, 6);
				Sleep(200);
			}
		}
	}
}

void BhopThread() {
	while (true) {
		if (GetAsyncKeyState(VK_SPACE) && BHop) {
			DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
			DWORD onGround = MemClass.readMem<DWORD>(player + offset.m_fFlags);

			if (player && onGround == 257) {
				MemClass.writeMem<int>(val.clientModule + offset.ForceJump, 5);
				Sleep(10);
				MemClass.writeMem<int>(val.clientModule + offset.ForceJump, 4);
			}
		}
	}
}

int main()
{
	system("CLS");
	SetConsoleTitleA("Triggered / Made by idan#1000");

	int procID = MemClass.getProcess("csgo.exe");

	val.clientModule = MemClass.getModule(procID, "client.dll");
	val.engineModule = MemClass.getModule(procID, "engine.dll"); 

	cout << WHITE << "* * * * * * * * * * * * * * * * * * *" << endl;
	cout << YELLOW << " _____     _                       _ " << endl;
	cout << RED << "|_   _|___|_|___ ___ ___ ___ ___ _| |" << endl;
	cout << BLUE << "  | | |  _| | . | . | -_|  _| -_| . |" << endl;
	cout << GREEN << "  |_| |_| |_|_  |_  |___|_| |___|___|" << endl;
	cout << RED << "            |___|___|v" << val.version << endl;
	cout << WHITE << "\n* * * * * * * * * * * * * * * * * * *\n" << endl;

	cout << BOLDYELLOW << "Triggerbot " << RED << "[OFF] " << BOLDBLUE << "[DEL]" << endl;
	cout << BOLDYELLOW << "GlowESP " << RED << "[OFF] " << BOLDBLUE << "[HOME]" << endl;
	cout << BOLDYELLOW << "ESP " << RED << "[OFF] " << BOLDBLUE << "[F7]" << endl;
	cout << BOLDYELLOW << "Aimbot " << RED << "[OFF] " << BOLDBLUE << "[F6]" << endl;
	cout << BOLDYELLOW << "Bunnyhop " << RED << "[OFF] " << BOLDBLUE << "[INS]\n" << endl;

	//Aimbot Thread
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FindClosestEnemyThread, NULL, NULL, NULL);

	//Triggerbot Thread
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TriggerbotThread, NULL, NULL, NULL);

	//Bhop Thread
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)BhopThread, NULL, NULL, NULL);


	while (true)
	{
		//BUNNYHOP KEY
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			BHop = !BHop;
			refreshState("BHOP" + BHop);
		}
		
		//TRIGGERBOT KEY
		if (GetAsyncKeyState(VK_DELETE) & 1) {
			Triggerbot = !Triggerbot;
			refreshState("Triggerbot" + Triggerbot);
		}

		//GLOWESP KEY
		if (GetAsyncKeyState(VK_HOME) & 1) {
			GlowESP = !GlowESP;
			refreshState("GlowESP" + GlowESP);
		}

		//ESP KEY
		if (GetAsyncKeyState(VK_F7) & 1) {
			ESP = !ESP;
			refreshState("ESP" + ESP);
		}

		//AIMBOT KEY
		if (GetAsyncKeyState(VK_F6) & 1) {
			Aimbot = !Aimbot;
			refreshState("Aimbot" + Aimbot);
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

		//ESP MODULE
		if (ESP) {
			view_matrix_tt vm = MemClass.readMem<view_matrix_tt>(val.clientModule + offset.dwViewMatrix);
			DWORD player = MemClass.readMem<DWORD>(val.clientModule + offset.dwLocalPlayer);
			DWORD localTeam = MemClass.readMem<DWORD>(player + offset.m_iTeamNum);
			
			for (int i = 1; i < 64; ++i)
			{
				//getName(i);

				uintptr_t pEnt = MemClass.readMem<DWORD>(val.clientModule + offset.dwEntityList + (i * 0x10));

				int health = MemClass.readMem<int>(pEnt + offset.m_iHealth);
				int team = MemClass.readMem<int>(pEnt + offset.m_iTeamNum);

				Vector3 pos = MemClass.readMem<Vector3>(pEnt + offset.m_vecOrigin);
				Vector3 head;
				head.x = pos.x;
				head.y = pos.y;
				head.z = pos.z + 75.f;
				Vector33 screenpos = WorldToScreenESP(pos, vm);
				Vector33 screenhead = WorldToScreenESP(head, vm);
				float height = screenhead.y - screenpos.y;
				float width = height / 2.4f;

				DWORD Entity = GetPlayer(i);
				int EnmHealth = GetPlayerHealth(Entity); if (EnmHealth < 1 || EnmHealth > 100) continue;
				int Dormant = DormantCheck(Entity); if (Dormant) continue;

				if (screenpos.z >= 0.01f && team != localTeam && health > 0 && health < 101 ) {
					DrawBorderBox(screenpos.x - (width / 2), screenpos.y, width, height, 2, "enemy");
					//DrawLine(screenX / 2, screenY, screenpos.x, screenpos.y, "enemy");
				}

				if (screenpos.z >= 0.01f && team == localTeam && health > 0 && health < 101) {
					DrawBorderBox(screenpos.x - (width / 2), screenpos.y, width, height, 2, "teammate");
					//DrawLine(screenX / 2, screenY, screenpos.x, screenpos.y, "teammate");
				}
			}
		}

		//AIMBOT MODULE
		if (Aimbot) {
			vm = MemClass.readMem<view_matrix_t>(val.clientModule + offset.dwViewMatrix);
			Vector3 closestw2shead = WorldToScreen(get_head(GetPlayer(closest)), vm);
			//DrawLine(xhairx, xhairy, closestw2shead.x, closestw2shead.y); //optinal for debugging
			//float height = closestw2shead.y - closestw2shead.y;
			//float width = height / 2.4f;
			//DrawBorderBox(closestw2shead.x - (width / 3), closestw2shead.y, width, height, 150, "enemy"); // Drawing red box on enemy's head


			if (GetAsyncKeyState(VK_MENU) && closestw2shead.z >= 0.001f)
				SetCursorPos(closestw2shead.x, closestw2shead.y);
		}

		//EXIT PROGRAM
		if (GetAsyncKeyState(VK_END) & 1) {
			refreshState("BHOP");
			refreshState("Triggerbot");
			refreshState("GlowESP");
			refreshState("ESP");
			refreshState("Aimbot");
			break;
		}
	}

	cout << "Shutting down.." << endl;
	Sleep(1000);

	return 0;
}
