#pragma once

#include "shared/statuseffects.h"

extern char* g_pLocalPlayerUserID;
extern char* g_pLocalPlayerOriginToken;

typedef void* (*GetBaseLocalClientType)();
extern GetBaseLocalClientType GetBaseLocalClient;

class C_Player
{
public:
	char unk_0x0[0x1AB8];
	StatusEffectTimedData m_statusEffectsTimedPlayerNV[10];
	StatusEffectEndlessData m_statusEffectsEndlessPlayerNV[10];
};

static_assert(offsetof(C_Player, m_statusEffectsTimedPlayerNV) == 0x1AB8);
static_assert(offsetof(C_Player, m_statusEffectsEndlessPlayerNV) == 0x1BA8);
