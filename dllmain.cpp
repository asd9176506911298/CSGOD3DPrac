#include "includes.h"
#include <sstream>
#include <string>

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

	
	DrawString("Yuki.kaco", windowWidth / 2, windowHeight - 20, D3DCOLOR_ARGB(255, 255, 255, 255));

	int menuOffX = windowWidth / 2;
	int menuOffY = 50;
	D3DCOLOR enable = D3DCOLOR_ARGB(255, 0, 255, 0);
	D3DCOLOR disbale = D3DCOLOR_ARGB(255, 255, 0, 0);

	if (!hack->settings.showMenu) {
		DrawString("Show Menu (INS)", menuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else {
		DrawString("Show Teamates (F1)", menuOffX, menuOffY + 0 * 12, hack->settings.showTeamates ? enable : disbale);
		DrawString("Snaplines (F2)", menuOffX, menuOffY + 1 * 12, hack->settings.snaplines ? enable : disbale);
		DrawString("2D Box (F3)", menuOffX, menuOffY + 2 * 12, hack->settings.box2D ? enable : disbale);
		DrawString("2D Statusbars (F4)", menuOffX, menuOffY + 3 * 12, hack->settings.status2D ? enable : disbale);
		DrawString("Status Text (F5)", menuOffX, menuOffY + 4 * 12, hack->settings.status2D ? enable : disbale);
		DrawString("3D Box (F6)", menuOffX, menuOffY + 5 * 12, hack->settings.box3D ? enable : disbale);
		DrawString("Velocity Esp (F7)", menuOffX, menuOffY + 6 * 12, hack->settings.velEsp ? enable : disbale);
		DrawString("Headline Esp (F8)", menuOffX, menuOffY + 7 * 12, hack->settings.headlineEsp ? enable : disbale);
		DrawString("Recoil crosshair (F9)", menuOffX, menuOffY + 8 * 12, hack->settings.rcsCrosshair? enable : disbale);
		DrawString("Hide Menu (INS)", menuOffX, menuOffY + 9 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	for (int i = 1; i < 16; i++) {
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt)) {
			continue;
		}
		
		D3DCOLOR espColor, snaplineColor, velocityColor, headlineColor;
		if (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum) {
			espColor = hack->color.team.esp;
			snaplineColor = hack->color.team.snapline;
			velocityColor = hack->color.team.velocity;
			headlineColor = hack->color.team.headline;
		}
		else {
			espColor = hack->color.enemy.esp;
			snaplineColor = hack->color.enemy.snapline;
			velocityColor = hack->color.enemy.velocity;
			headlineColor = hack->color.enemy.headline;
		}

		if(!hack->settings.showTeamates && (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum))
			continue;

		Vec3 entHead3D = hack->GetBonePos(curEnt, 8);
		entHead3D.z += 8;
		Vec2 entPos2D, entHead2D;
		// snapline
		if (hack->WorldToScreen(curEnt->m_vecOrigin, entPos2D)){
			if (hack->settings.velEsp) {
				Vec3 velOff = curEnt->m_vecOrigin + (curEnt->m_vecVelocity * .25);
				Vec2 velOff2D;

				if (hack->WorldToScreen(velOff, velOff2D)) {
					DrawLine(entPos2D, velOff2D, 2, velocityColor);
					DrawFilledRect(velOff2D.x - 2, velOff2D.y - 2, 4, 4, espColor);
				}
			}

			if (hack->settings.snaplines) {
				DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, snaplineColor);
			}
				
			//DrawBox
			if (hack->WorldToScreen(entHead3D, entHead2D)){
				if (hack->settings.box2D) {
					DrawEspBox2D(entPos2D, entHead2D, 2, espColor);
				}

				if (hack->settings.box3D) {
					DrawEspBox3D(entHead3D, curEnt->m_vecOrigin, curEnt->m_angEyeAnglesY, 25, 2, espColor);
				}
					
				if (hack->settings.status2D) {
					int height = ABS(entPos2D.y - entHead2D.y);
					int dX = (entPos2D.x - entHead2D.x);

					float healthPerc = curEnt->m_iHealth / 100.f;
					float armorPerc = curEnt->m_ArmorValue / 100.f;

					Vec2 botHealth, topHealth, botArmor, topArmor;
					int healthHeight = height * healthPerc;
					int armorHeight = height * armorPerc;

					botHealth.y = botArmor.y = entPos2D.y;

					botHealth.x = entPos2D.x - (height / 4) - 2;
					botArmor.x = entPos2D.x + (height / 4) + 2;

					topHealth.y = entHead2D.y + height - healthHeight;
					topArmor.y = entHead2D.y + height - armorHeight;

					topHealth.x = entPos2D.x - (height / 4) - 2 - (dX * healthPerc);
					topArmor.x = entPos2D.x + (height / 4) + 2 - (dX * armorPerc);

					DrawLine(botHealth, topHealth, 2, hack->color.health);
					DrawLine(botArmor, topArmor, 2, hack->color.armor);
				}

				if (hack->settings.headlineEsp) {
					Vec3 head3D = hack->GetBonePos(curEnt, 8);
					Vec3 entAngles;
					entAngles.x = -curEnt->m_angEyeAnglesX;
					entAngles.y = curEnt->m_angEyeAnglesY;
					entAngles.z = 0;
					Vec3 entPoint = hack->TransformVec(head3D, entAngles, 60);
					Vec2 endPoint2d, head2D;
					hack->WorldToScreen(head3D, head2D);
					if(hack->WorldToScreen(entPoint, endPoint2d)) {
						DrawLine(head2D, endPoint2d, 2, headlineColor);
					}
				}

				if (hack->settings.statusText) {
					std::stringstream s1, s2;
					s1 << curEnt->m_iHealth;
					s2 << curEnt->m_ArmorValue;
					std::string t1 = "hp: " + s1.str();
					std::string t2 = "ap: " + s2.str();

					char* healthMsg = (char*)t1.c_str();
					char* armorMsg = (char*)t2.c_str();

					DrawString(healthMsg, entPos2D.x, entPos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
					DrawString(armorMsg, entPos2D.x, entPos2D.y + 12, D3DCOLOR_ARGB(255, 255, 255, 255));

					if (curEnt->m_bHasHelmet) {
						DrawString("has helmet", entPos2D.x, entPos2D.y + 24, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}

				
			}
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

	if (hack->settings.rcsCrosshair)
	{
		Vec2 l, r, t, b;
		l = r = t = b = hack->crosshair2D;
		l.x -= hack->crosshairSize;
		r.x += hack->crosshairSize;
		b.y += hack->crosshairSize;
		t.y -= hack->crosshairSize;

		DrawLine(l, r, 2, hack->color.crosshair);
		DrawLine(t, b, 2, hack->color.crosshair);
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

		Vec3 pAng = hack->localEnt->m_aimPunchAngle;

		hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		hack->crosshair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);

		//if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		//	boneId = !boneId;
		//if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		//	DrawBone = !DrawBone;
	}

	//unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	Sleep(1000);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}