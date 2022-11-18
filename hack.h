#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
};

struct Vec4 {
	float x, y, z, w;
};

class Ent {
public:
	union 
	{
		DEFINE_MEMBER_N(bool, m_bDormant, 0xED);
		DEFINE_MEMBER_N(int, m_iHealth, 0x100);
		DEFINE_MEMBER_N(Vec3, m_vecOrigin, 0x138);
		DEFINE_MEMBER_N(int, m_iTeamNum, 0xF4);
		DEFINE_MEMBER_N(int, m_dwBoneMatrix, 0x26A8);
	};
};

class EntListObj {
public:
	Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = 0x4DFBE74;
	uintptr_t dwViewMatrix = 0x4DECCA4;

	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	void Init();
	void Update();
	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int boneid);
	void DrawBone(Ent* ent, int boneid1, int boneid2);
};