#include "includes.h"

Hack::~Hack() {
	this->FontF->Release();
	this->LineL->Release();
}

void Hack::Init() {
	client = (uintptr_t)(GetModuleHandle("client.dll"));
	engine = (uintptr_t)(GetModuleHandle("engine.dll"));
	entList = (EntList*)(client + dwEntityList);
	localEnt = entList->ents[0].ent;
}

void Hack::Update() {
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix));
	this->CheckButtons();
}

void Hack::CheckButtons() {
	if (GetAsyncKeyState(button.showMenuBtn) & 1)
		settings.showMenu = !settings.showMenu;
	if (GetAsyncKeyState(button.showTeamatesBtn) & 1)
		settings.showTeamates = !settings.showTeamates;
	if (GetAsyncKeyState(button.snaplinesBtn) & 1)
		settings.snaplines = !settings.snaplines;
	if (GetAsyncKeyState(button.box2DBtn) & 1)
		settings.box2D = !settings.box2D;
	if (GetAsyncKeyState(button.status2DBtn) & 1)
		settings.status2D = !settings.status2D;
	if (GetAsyncKeyState(button.statusTextBtn) & 1)
		settings.statusText = !settings.statusText;
	if (GetAsyncKeyState(button.box3DBtn) & 1)
		settings.box3D = !settings.box3D;
	if (GetAsyncKeyState(button.velEspBtn) & 1)
		settings.velEsp = !settings.velEsp;
	if (GetAsyncKeyState(button.headlineEspBtn) & 1)
		settings.headlineEsp = !settings.headlineEsp;
	if (GetAsyncKeyState(button.rcsCrosshairBtn) & 1)
		settings.rcsCrosshair = !settings.rcsCrosshair;
}

bool Hack::CheckValidEnt(Ent* ent) {
	if (ent == nullptr)
		return false;
	if (ent == localEnt)
		return false;
	if (ent->m_iHealth <= 0)
		return false;
	if (ent->m_bDormant)
		return false;
	return true;
}

bool Hack::WorldToScreen(Vec3 pos, Vec2& screen) {
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];


	if (clipCoords.w < 0.1f)
	{
		return false;
	}

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;


	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	return true;
}

Vec3 Hack::GetBonePos(Ent* ent, int boneid) {
	uintptr_t bonePtr = ent->m_dwBoneMatrix;
	Vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * boneid + 0xC);
	bonePos.y = *(float*)(bonePtr + 0x30 * boneid + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * boneid + 0x2C);
	return bonePos;
}

void Hack::DrawBone(Ent* ent, int boneid1, int boneid2) {
	Vec2 Bone1Pos2D, Bone2Pos2D;
	if (WorldToScreen(GetBonePos(ent, boneid1), Bone1Pos2D))
	{
		if (WorldToScreen(GetBonePos(ent, boneid2), Bone2Pos2D))
		{
			DrawLine(Bone1Pos2D, Bone2Pos2D, 2, D3DCOLOR_ARGB(255, 255, 0, 0));
		}
	}
}

Vec3 Hack::TransformVec(Vec3 src, Vec3 ang, float d) {
	Vec3 newPos;
	newPos.x = src.x + (cosf(TORAD(ang.y)) * d);
	newPos.y = src.y + (sinf(TORAD(ang.y)) * d);
	newPos.z = src.z + (tanf(TORAD(ang.x)) * d);
	return newPos;
}