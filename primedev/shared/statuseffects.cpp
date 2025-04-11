#include "statuseffects.h"
#include "squirrel/squirrel.h"
#include "client/r2client.h"
#include "server/r2server.h"

void Sv_PushStatusEffects(HSquirrelVM* sqvm)
{
	const ScriptContext context = ScriptContext::SERVER;
	const CBasePlayer* pPlayer = g_pSquirrel<context>->getentity<CBasePlayer>(sqvm, 1);

	g_pSquirrel<context>->newarray(sqvm, 0);
	for (int i = 0; i < 10; i++)
	{
		StatusEffectTimedData effect;
		effect = pPlayer->m_statusEffectsTimedPlayerNV[i];

		float intensity = ((effect.seComboVars >> 8) % (1 << 8)) / 255.0;
		if (intensity == 0)
			continue;

		g_pSquirrel<context>->pushnewstructinstance(sqvm, 6);

		g_pSquirrel<context>->pushbool(sqvm, false); // isEndless
		g_pSquirrel<context>->sealstructslot(sqvm, 0);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 26)); // id
		g_pSquirrel<context>->sealstructslot(sqvm, 1);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 16) % (1 << 10)); // instanceId
		g_pSquirrel<context>->sealstructslot(sqvm, 2);

		g_pSquirrel<context>->pushfloat(sqvm, intensity); // intensity
		g_pSquirrel<context>->sealstructslot(sqvm, 3);

		g_pSquirrel<context>->pushfloat(sqvm, effect.seTimeEnd); // endTime
		g_pSquirrel<context>->sealstructslot(sqvm, 4);

		g_pSquirrel<context>->pushfloat(sqvm, effect.seFadeOutTime); // fadeOutTime
		g_pSquirrel<context>->sealstructslot(sqvm, 5);

		g_pSquirrel<context>->arrayappend(sqvm, -2);

		// spdlog::info("Player status effect {} {}, end time {}", effect.seComboVars, effect.seComboVars2, effect.seTimeEnd);
	}

	for (int i = 0; i < 10; i++)
	{
		StatusEffectEndlessData effect;
		effect = pPlayer->m_statusEffectsEndlessPlayerNV[i];

		float intensity = ((effect.seComboVars >> 8) % (1 << 8)) / 255.0;
		if (intensity == 0)
			continue;

		g_pSquirrel<context>->pushnewstructinstance(sqvm, 6);

		g_pSquirrel<context>->pushbool(sqvm, true); // isEndless
		g_pSquirrel<context>->sealstructslot(sqvm, 0);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 26)); // id
		g_pSquirrel<context>->sealstructslot(sqvm, 1);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 16) % (1 << 10)); // instanceId
		g_pSquirrel<context>->sealstructslot(sqvm, 2);

		g_pSquirrel<context>->pushfloat(sqvm, intensity); // intensity
		g_pSquirrel<context>->sealstructslot(sqvm, 3);

		g_pSquirrel<context>->pushfloat(sqvm, -1); // endTime
		g_pSquirrel<context>->sealstructslot(sqvm, 4);

		g_pSquirrel<context>->pushfloat(sqvm, -1); // fadeOutTime
		g_pSquirrel<context>->sealstructslot(sqvm, 5);

		g_pSquirrel<context>->arrayappend(sqvm, -2);
	}
}

void Cl_PushStatusEffects(HSquirrelVM* sqvm)
{
	const ScriptContext context = ScriptContext::CLIENT;
	const C_Player* pPlayer = g_pSquirrel<context>->getentity<C_Player>(sqvm, 1);

	g_pSquirrel<context>->newarray(sqvm, 0);
	for (int i = 0; i < 10; i++)
	{
		StatusEffectTimedData effect;
		effect = pPlayer->m_statusEffectsTimedPlayerNV[i];

		float intensity = ((effect.seComboVars >> 8) % (1 << 8)) / 255.0;
		if (intensity == 0)
			continue;

		g_pSquirrel<context>->pushnewstructinstance(sqvm, 6);

		g_pSquirrel<context>->pushbool(sqvm, false); // isEndless
		g_pSquirrel<context>->sealstructslot(sqvm, 0);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 26)); // id
		g_pSquirrel<context>->sealstructslot(sqvm, 1);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 16) % (1 << 10)); // instanceId
		g_pSquirrel<context>->sealstructslot(sqvm, 2);

		g_pSquirrel<context>->pushfloat(sqvm, intensity); // intensity
		g_pSquirrel<context>->sealstructslot(sqvm, 3);

		g_pSquirrel<context>->pushfloat(sqvm, effect.seTimeEnd); // endTime
		g_pSquirrel<context>->sealstructslot(sqvm, 4);

		g_pSquirrel<context>->pushfloat(sqvm, effect.seFadeOutTime); // fadeOutTime
		g_pSquirrel<context>->sealstructslot(sqvm, 5);

		g_pSquirrel<context>->arrayappend(sqvm, -2);

		// spdlog::info("Player status effect {} {}, end time {}", effect.seComboVars, effect.seComboVars2, effect.seTimeEnd);
	}

	for (int i = 0; i < 10; i++)
	{
		StatusEffectEndlessData effect;
		effect = pPlayer->m_statusEffectsEndlessPlayerNV[i];

		float intensity = ((effect.seComboVars >> 8) % (1 << 8)) / 255.0;
		if (intensity == 0)
			continue;

		g_pSquirrel<context>->pushnewstructinstance(sqvm, 6);

		g_pSquirrel<context>->pushbool(sqvm, true); // isEndless
		g_pSquirrel<context>->sealstructslot(sqvm, 0);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 26)); // id
		g_pSquirrel<context>->sealstructslot(sqvm, 1);

		g_pSquirrel<context>->pushinteger(sqvm, (effect.seComboVars >> 16) % (1 << 10)); // instanceId
		g_pSquirrel<context>->sealstructslot(sqvm, 2);

		g_pSquirrel<context>->pushfloat(sqvm, intensity); // intensity
		g_pSquirrel<context>->sealstructslot(sqvm, 3);

		g_pSquirrel<context>->pushfloat(sqvm, -1); // endTime
		g_pSquirrel<context>->sealstructslot(sqvm, 4);

		g_pSquirrel<context>->pushfloat(sqvm, -1); // fadeOutTime
		g_pSquirrel<context>->sealstructslot(sqvm, 5);

		g_pSquirrel<context>->arrayappend(sqvm, -2);
	}
}

ADD_SQFUNC("array<StatusEffectData>", GetPlayerStatusEffects, "entity player", "", ScriptContext::CLIENT | ScriptContext::SERVER)
{
	if (context == ScriptContext::SERVER)
		Sv_PushStatusEffects(sqvm);
	else
		Cl_PushStatusEffects(sqvm);

	return SQRESULT_NOTNULL;
}

ADD_SQFUNC("void", DumpCallstack, "", "", ScriptContext::UI | ScriptContext::SERVER | ScriptContext::CLIENT)
{
	spdlog::info("Callstack:");

	PVOID pFrames[32];

	int iFrames = RtlCaptureStackBackTrace(0, 32, pFrames, NULL);

	for (int i = 0; i < iFrames; i++)
	{
		const CHAR* pszModuleFileName;

		LPCSTR pAddress = static_cast<LPCSTR>(pFrames[i]);
		HMODULE hModule;
		if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, pAddress, &hModule))
		{
			pszModuleFileName = "GetModuleHandleExA failed!";
			// If we fail here it's too late to do any damage control
		}
		else
		{
			CHAR szModulePath[MAX_PATH];
			GetModuleFileNameExA(GetCurrentProcess(), hModule, szModulePath, sizeof(szModulePath));
			pszModuleFileName = strrchr(szModulePath, '\\') + 1;
		}

		// Get relative address
		LPCSTR pCrashOffset = reinterpret_cast<LPCSTR>(pAddress - reinterpret_cast<LPCSTR>(hModule));
		std::string svCrashOffset = fmt::format("{:#x}", reinterpret_cast<DWORD64>(pCrashOffset));

		// Log module + offset
		spdlog::info("\t{} + {:#x}", pszModuleFileName, reinterpret_cast<DWORD64>(pCrashOffset));
	}
	return SQRESULT_NULL;
}

ADD_SQFUNC("void", PrintEntityAddress, "entity ent", "", ScriptContext::SERVER | ScriptContext::CLIENT)
{
	const void* pPlayer = g_pSquirrel<context>->getentity<void>(sqvm, 1);
	spdlog::info("Entity Address: {}", pPlayer);
	return SQRESULT_NULL;
}
