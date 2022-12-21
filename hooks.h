#pragma once
#include "main.h"

enum Indexes : int
{
	CREATEMOVE = 25,
	FRAMESTAGENOTIFY = 37,
	PAINTTRAVERSE = 41
};

extern CVMTHookManager* clienthook;
extern CVMTHookManager* clientmodehook;
extern CVMTHookManager* panelhook;

void __fastcall FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t stage );
bool __fastcall CreateMove( void* ecx, void* edx, float frametime, CUserCmd* cmd );
void __fastcall PaintTraverse( void* ecx, void* edx, unsigned int panelid, bool repaint, bool force );