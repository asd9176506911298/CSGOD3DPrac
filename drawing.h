#pragma once
#include "hack.h"

extern ID3DXFont* FontF;

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color);

void DrawString(const char* text, int x, int y, D3DCOLOR color);

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);

void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color);