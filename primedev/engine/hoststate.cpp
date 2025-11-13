#include "engine/hoststate.h"
#include "server/auth/serverauthentication.h"
#include "server/serverpresence.h"
#include "shared/playlist.h"
#include "core/tier0.h"
#include "engine/r2engine.h"
#include "squirrel/squirrel.h"
#include "plugins/pluginmanager.h"
#include "core/hooks.h"

AUTOHOOK_INIT()

CHostState* g_pHostState;

std::string sLastMode;

VAR_AT(engine.dll + 0x13FA6070, ConVar*, Cvar_hostport);
FUNCTION_AT(engine.dll + 0x1232C0, void, __fastcall, _Cmd_Exec_f, (const CCommand& arg, bool bOnlyIfExists, bool bUseWhitelists));

void ServerStartingOrChangingMap()
{
	ConVar* Cvar_mp_gamemode = g_pCVar->FindVar("mp_gamemode");

	// net_data_block_enabled is required for sp, force it if we're on an sp map
	// sucks for security but just how it be
	if (!strncmp(g_pHostState->m_levelName, "sp_", 3))
	{
		g_pCVar->FindVar("net_data_block_enabled")->SetValue(true);
		g_pServerAuthentication->m_bStartingLocalSPGame = true;
	}
	else
		g_pServerAuthentication->m_bStartingLocalSPGame = false;
}

// clang-format off
AUTOHOOK(CHostState__State_NewGame, engine.dll + 0x16E7D0,
void, __fastcall, (CHostState* self))
// clang-format on
{
	spdlog::info("HostState: NewGame");

	ServerStartingOrChangingMap();

	double dStartTime = Plat_FloatTime();
	CHostState__State_NewGame(self);
	spdlog::info("loading took {}s", Plat_FloatTime() - dStartTime);
}

// clang-format off
AUTOHOOK(CHostState__State_LoadGame, engine.dll + 0x16E730,
void, __fastcall, (CHostState* self))
// clang-format on
{
	// singleplayer server starting
	// useless in 99% of cases but without it things could potentially break very much

	spdlog::info("HostState: LoadGame");

	Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec autoexec_ns_server", cmd_source_t::kCommandSrcCode);
	Cbuf_Execute();

	// this is normally done in ServerStartingOrChangingMap(), but seemingly the map name isn't set at this point
	g_pCVar->FindVar("net_data_block_enabled")->SetValue(true);
	g_pServerAuthentication->m_bStartingLocalSPGame = true;

	double dStartTime = Plat_FloatTime();
	CHostState__State_LoadGame(self);
	spdlog::info("loading took {}s", Plat_FloatTime() - dStartTime);
}

// clang-format off
AUTOHOOK(CHostState__State_ChangeLevelMP, engine.dll + 0x16E520,
void, __fastcall, (CHostState* self))
// clang-format on
{
	spdlog::info("HostState: ChangeLevelMP");

	ServerStartingOrChangingMap();

	double dStartTime = Plat_FloatTime();
	CHostState__State_ChangeLevelMP(self);
	spdlog::info("loading took {}s", Plat_FloatTime() - dStartTime);
}

// clang-format off
AUTOHOOK(CHostState__State_GameShutdown, engine.dll + 0x16E640,
void, __fastcall, (CHostState* self))
// clang-format on
{
	spdlog::info("HostState: GameShutdown");

	CHostState__State_GameShutdown(self);

	// run gamemode cleanup cfg now instead of when we start next map
	if (sLastMode.length())
	{
		char* commandBuf[1040]; // assumedly this is the size of CCommand since we don't have an actual constructor
		memset(commandBuf, 0, sizeof(commandBuf));
		CCommand tempCommand = *(CCommand*)&commandBuf;
		if (CCommand__Tokenize(
				tempCommand, fmt::format("exec server/cleanup_gamemode_{}", sLastMode).c_str(), cmd_source_t::kCommandSrcCode))
		{
			_Cmd_Exec_f(tempCommand, false, false);
			Cbuf_Execute();
		}

		sLastMode.clear();
	}
}

// clang-format off
AUTOHOOK(CHostState__FrameUpdate, engine.dll + 0x16DB00,
void, __fastcall, (CHostState* self, double flCurrentTime, float flFrameTime))
// clang-format on
{
	CHostState__FrameUpdate(self, flCurrentTime, flFrameTime);

	// Run Squirrel message buffer
	if (g_pSquirrel<ScriptContext::UI>->m_pSQVM != nullptr && g_pSquirrel<ScriptContext::UI>->m_pSQVM->sqvm != nullptr)
		g_pSquirrel<ScriptContext::UI>->ProcessMessageBuffer();

	if (g_pSquirrel<ScriptContext::CLIENT>->m_pSQVM != nullptr && g_pSquirrel<ScriptContext::CLIENT>->m_pSQVM->sqvm != nullptr)
		g_pSquirrel<ScriptContext::CLIENT>->ProcessMessageBuffer();

	if (g_pSquirrel<ScriptContext::SERVER>->m_pSQVM != nullptr && g_pSquirrel<ScriptContext::SERVER>->m_pSQVM->sqvm != nullptr)
		g_pSquirrel<ScriptContext::SERVER>->ProcessMessageBuffer();

	g_pPluginManager->RunFrame();
}

ON_DLL_LOAD_RELIESON("engine.dll", HostState, ConVar, (CModule module))
{
	AUTOHOOK_DISPATCH()

	g_pHostState = module.Offset(0x7CF180).RCast<CHostState*>();
}
