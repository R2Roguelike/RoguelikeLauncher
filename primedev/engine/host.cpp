#include "core/convar/convar.h"
#include "mods/modmanager.h"
#include "util/printcommands.h"
#include "util/printmaps.h"
#include "shared/misccommands.h"
#include "r2engine.h"
#include "core/tier0.h"

AUTOHOOK_INIT()

// clang-format off
AUTOHOOK(Host_Init, engine.dll + 0x155EA0,
void, __fastcall, (bool bDedicated))
// clang-format on
{
	spdlog::info("Host_Init()");
	Host_Init(bDedicated);
	FixupCvarFlags();
	// need to initialise these after host_init since they do stuff to preexisting concommands/convars without being client/server specific
	InitialiseCommandPrint();
	InitialiseMapsPrint();
}

// ensure that GetLocalBaseClient().m_bRestrictServerCommands is set correctly, which the return value of this function controls
// this is IsValveMod in source, but we're making it IsRespawnMod now since valve didn't make this one
// clang-format off
AUTOHOOK(IsRespawnMod, engine.dll + 0x1C6360,
bool, __fastcall, (const char* pModName)) // 48 83 EC 28 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 85 C0 74 63
// clang-format on
{
	spdlog::info("MOD NAME {}", pModName);
	// somewhat temp, store the modname here, since we don't have a proper ptr in engine to it rn
	size_t iSize = strlen(pModName);
	g_pModName = new char[iSize + 1];
	strcpy(g_pModName, pModName);

	return (false);
}

ON_DLL_LOAD("engine.dll", Host_Init, (CModule module))
{
	AUTOHOOK_DISPATCH()
}
