#include "squirrel/squirrel.h"
#include "server/auth/serverauthentication.h"
#include "client/r2client.h"
#include "server/r2server.h"

#include <filesystem>

ADD_SQFUNC("bool", NSIsPlayerLocalPlayer, "entity player", "", ScriptContext::SERVER)
{
	const CBasePlayer* pPlayer = g_pSquirrel<ScriptContext::SERVER>->template getentity<CBasePlayer>(sqvm, 1);
	if (!pPlayer)
	{
		spdlog::warn("NSIsPlayerLocalPlayer got null player");

		g_pSquirrel<context>->pushbool(sqvm, false);
		return SQRESULT_NOTNULL;
	}

	CBaseClient* pClient = &g_pClientArray[pPlayer->m_nPlayerIndex - 1];
	g_pSquirrel<context>->pushbool(sqvm, !strcmp(g_pLocalPlayerUserID, pClient->m_UID));
	return SQRESULT_NOTNULL;
}

// script Roguelike_Player_SetDodgeSpeed(__p(), 1)
ADD_SQFUNC("void", Roguelike_Player_SetDodgeSpeed, "entity player, float val", "", ScriptContext::SERVER | ScriptContext::CLIENT)
{
	const CMemory pPlayer = new CMemory(g_pSquirrel<context>->template getentity<CBasePlayer>(sqvm, 1));


	CMemory dodgeSpeed = pPlayer.Deref().Offset(context == ScriptContext::SERVER ? 0x1D30 : 0x2050);

	spdlog::warn("{}", dodgeSpeed.GetPtr());

	*(dodgeSpeed.CCast<float*>()) = g_pSquirrel<context>->getfloat(sqvm, 2);
	
	return SQRESULT_NOTNULL;
}
ADD_SQFUNC("void", Roguelike_Player_SetDodgeDrain, "entity player, float val", "", ScriptContext::SERVER | ScriptContext::CLIENT)
{
	const CMemory pPlayer = new CMemory(g_pSquirrel<context>->template getentity<void>(sqvm, 1));

	CMemory dodgeSpeed = pPlayer.Deref().Offset(context == ScriptContext::SERVER ? 0x1D34 : 0x2054);

	spdlog::warn("{}", dodgeSpeed.GetPtr());

	*(dodgeSpeed.CCast<float*>()) = g_pSquirrel<context>->getfloat(sqvm, 2);

	return SQRESULT_NOTNULL;
}

ADD_SQFUNC(
	"bool",
	NSDisconnectPlayer,
	"entity player, string reason",
	"Disconnects the player from the server with the given reason",
	ScriptContext::SERVER)
{
	const CBasePlayer* pPlayer = g_pSquirrel<context>->template getentity<CBasePlayer>(sqvm, 1);
	const char* reason = g_pSquirrel<context>->getstring(sqvm, 2);

	if (!pPlayer)
	{
		spdlog::warn("Attempted to call NSDisconnectPlayer() with null player.");

		g_pSquirrel<context>->pushbool(sqvm, false);
		return SQRESULT_NOTNULL;
	}

	// Shouldn't happen but I like sanity checks.
	CBaseClient* pClient = &g_pClientArray[pPlayer->m_nPlayerIndex - 1];
	if (!pClient)
	{
		spdlog::warn("NSDisconnectPlayer(): player entity has null CBaseClient!");

		g_pSquirrel<context>->pushbool(sqvm, false);
		return SQRESULT_NOTNULL;
	}

	if (reason)
	{
		CBaseClient__Disconnect(pClient, 1, reason);
	}
	else
	{
		CBaseClient__Disconnect(pClient, 1, "Disconnected by the server.");
	}

	g_pSquirrel<context>->pushbool(sqvm, true);
	return SQRESULT_NOTNULL;
}
