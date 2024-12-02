#pragma once

#pragma pack(push, 1)
struct StatusEffectTimedData
{
	void* vftable;
	unsigned int seComboVars;
	float seTimeEnd;
	float seFadeOutTime;
	int pad;
};
#pragma pack(pop)
static_assert(sizeof(StatusEffectTimedData) == 24); // 0x18

#pragma pack(push, 1)
struct StatusEffectEndlessData
{
	void* vftable;
	unsigned int seComboVars;
	unsigned int seComboVars2;
};
#pragma pack(pop)
static_assert(sizeof(StatusEffectEndlessData) == 16); // 0x10
