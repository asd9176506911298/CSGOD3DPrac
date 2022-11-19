#include "includes.h"

// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;

Hack* hack;

bool boneId = false;
bool DrawBone = false;


// hook function
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;

	
	DrawFilledRect(25, 25, 100, 100, D3DCOLOR_ARGB(255, 255, 255, 255));

	DrawString("Hello World", 100, 100, D3DCOLOR_ARGB(255, 255, 0, 0));

	for (int i = 1; i < 16; i++) {
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt)) {
			continue;
		}
		
		D3DCOLOR color;
		if (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum)
			color = D3DCOLOR_ARGB(255, 0, 255, 0);
		else
			color = D3DCOLOR_ARGB(255, 255, 0, 0);
		Vec2 entPos2D;
		if (hack->WorldToScreen(curEnt->m_vecOrigin, entPos2D))
		{
			DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, color);
		}

		//if(boneId)
		//{
		//	for(int x = 0; x < 80; x++)
		//	{
		//		Vec2 BonePos2D;
		//		std::string tmp = std::to_string(x);
		//		char const* boneId = tmp.c_str();
		//		if (hack->WorldToScreen(hack->GetBonePos(curEnt, x), BonePos2D)) 
		//		{
		//			DrawString(boneId, BonePos2D.x, BonePos2D.y, D3DCOLOR_ARGB(255, 255, 0, 0));
		//		}
		//	}
		//}
		//
		//if(DrawBone)
		//{
		//	if(!curEnt->m_bDormant)
		//	{
		//		//head
		//		hack->DrawBone(curEnt, 8, 0);
		//		if (curEnt->m_iTeamNum == 2) {
		//			//left arm
		//			hack->DrawBone(curEnt, 7, 39);
		//			hack->DrawBone(curEnt, 39, 40);
		//			hack->DrawBone(curEnt, 40, 63);
		//			//right arm
		//			hack->DrawBone(curEnt, 7, 11);
		//			hack->DrawBone(curEnt, 11, 12);
		//			hack->DrawBone(curEnt, 12, 35);
		//			//left leg
		//			hack->DrawBone(curEnt, 0, 74);
		//			hack->DrawBone(curEnt, 74, 75);
		//			//right leg
		//			hack->DrawBone(curEnt, 0, 67);
		//			hack->DrawBone(curEnt, 67, 68);
		//		}
		//		else if (curEnt->m_iTeamNum == 3)
		//		{
		//			//left arm
		//			hack->DrawBone(curEnt, 7, 68);
		//			hack->DrawBone(curEnt, 68, 42);
		//			hack->DrawBone(curEnt, 42, 65);
		//			//right arm
		//			hack->DrawBone(curEnt, 7, 38);
		//			hack->DrawBone(curEnt, 38, 12);
		//			hack->DrawBone(curEnt, 12, 35);
		//			//left leg
		//			hack->DrawBone(curEnt, 0, 78);
		//			hack->DrawBone(curEnt, 78, 79);
		//			//right leg
		//			hack->DrawBone(curEnt, 0, 71);
		//			hack->DrawBone(curEnt, 71, 72);
		//		}
		//	}
		//	
		//}
	}

	oEndScene(pDevice);
}


DWORD WINAPI HackThread(HMODULE hModule) {
	
	//hook
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	hack = new Hack();
	hack->Init();

	while (!GetAsyncKeyState(VK_END)) {
		hack->Update();
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			boneId = !boneId;
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
			DrawBone = !DrawBone;
	}

	//unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}