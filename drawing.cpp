#include "includes.h"

extern Hack* hack;

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawString(const char* text, int x, int y, D3DCOLOR color){
	RECT rect;

	if (!hack->FontF)
		D3DXCreateFont(pDevice, 24, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &hack->FontF);

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
}

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color) {
	if(!hack->LineL)
		D3DXCreateLine(pDevice, &hack->LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	hack->LineL->SetWidth(thickness);
	hack->LineL->Draw(Line, 2, color);
}

void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color) {
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}