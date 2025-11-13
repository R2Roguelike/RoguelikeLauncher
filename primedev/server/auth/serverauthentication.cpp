#include "serverauthentication.h"
#include "core/convar/cvar.h"
#include "core/convar/convar.h"
#include "server/serverpresence.h"
#include "engine/hoststate.h"
#include "core/convar/concommand.h"
#include "config/profile.h"
#include "core/tier0.h"
#include "engine/r2engine.h"
#include "client/r2client.h"
#include "server/r2server.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <thread>

AUTOHOOK_INIT()

// global vars
ServerAuthenticationManager* g_pServerAuthentication;
CBaseServer__RejectConnectionType CBaseServer__RejectConnection;

// auth hooks

// store these in vars so we can use them in CBaseClient::Connect
// this is fine because ptrs won't decay by the time we use this, just don't use it outside of calls from cbaseclient::connectclient
char* pNextPlayerToken;
uint64_t iNextPlayerUid;

// clang-format off
AUTOHOOK(CBaseServer__ConnectClient, engine.dll + 0x114430,
void*,, (
	void* self,
	void* addr,
	void* a3,
	uint32_t a4,
	uint32_t a5,
	int32_t a6,
	void* a7,
	char* playerName,
	char* serverFilter,
	void* a10,
	char a11,
	void* a12,
	char a13,
	char a14,
	int64_t uid,
	uint32_t a16,
	uint32_t a17))
// clang-format on
{
	// auth tokens are sent with serverfilter, can't be accessed from player struct to my knowledge, so have to do this here
	pNextPlayerToken = serverFilter;
	iNextPlayerUid = uid;

	return CBaseServer__ConnectClient(self, addr, a3, a4, a5, a6, a7, playerName, serverFilter, a10, a11, a12, a13, a14, uid, a16, a17);
}

ConVar* Cvar_ns_allowuserclantags;

// clang-format off
AUTOHOOK(CBaseClient__Connect, engine.dll + 0x101740,
bool,, (CBaseClient* self, char* pName, void* pNetChannel, char bFakePlayer, void* a5, char pDisconnectReason[256], void* a7))
// clang-format on
{
	char pVerifiedName[64];

	// try to actually connect the player
	if (!CBaseClient__Connect(self, pName, pNetChannel, bFakePlayer, a5, pDisconnectReason, a7))
		return false;

	return true;
}

// clang-format off
AUTOHOOK(CBaseClient__ActivatePlayer, engine.dll + 0x100F80,
void,, (CBaseClient* self))
// clang-format on
{
	CBaseClient__ActivatePlayer(self);
}

// clang-format off
AUTOHOOK(_CBaseClient__Disconnect, engine.dll + 0x1012C0,
void,, (CBaseClient* self, uint32_t unknownButAlways1, const char* pReason, ...))
// clang-format on
{
	// have to manually format message because can't pass varargs to original func
	char buf[1024];

	va_list va;
	va_start(va, pReason);
	vsprintf(buf, pReason, va);
	va_end(va);

	// this reason is used while connecting to a local server, hacky, but just ignore it
	if (strcmp(pReason, "Connection closing"))
	{
		spdlog::info("Player {} disconnected: \"{}\"", self->m_Name, buf);

		memset(self->m_PersistenceBuffer, 0, g_pServerAuthentication->m_PlayerAuthenticationData[self].pdataSize);
	}

	_CBaseClient__Disconnect(self, unknownButAlways1, buf);
}

ON_DLL_LOAD_RELIESON("engine.dll", ServerAuthentication, (ConCommand, ConVar), (CModule module))
{
	AUTOHOOK_DISPATCH()

	g_pServerAuthentication = new ServerAuthenticationManager;

	g_pServerAuthentication->Cvar_ns_erase_auth_info =
		new ConVar("ns_erase_auth_info", "1", FCVAR_GAMEDLL, "Whether auth info should be erased from this server on disconnect or crash");
	g_pServerAuthentication->Cvar_ns_auth_allow_insecure =
		new ConVar("ns_auth_allow_insecure", "0", FCVAR_GAMEDLL, "Whether this server will allow unauthenicated players to connect");
	g_pServerAuthentication->Cvar_ns_auth_allow_insecure_write = new ConVar(
		"ns_auth_allow_insecure_write",
		"0",
		FCVAR_GAMEDLL,
		"Whether the pdata of unauthenticated clients will be written to disk when changed");

	// patch to disable kicking based on incorrect serverfilter in connectclient, since we repurpose it for use as an auth token
	module.Offset(0x114655).Patch("EB");

	// patch to disable fairfight marking players as cheaters and kicking them
	module.Offset(0x101012).Patch("E9 90 00");

	CBaseServer__RejectConnection = module.Offset(0x1182E0).RCast<CBaseServer__RejectConnectionType>();

	if (CommandLine()->CheckParm("-allowdupeaccounts"))
	{
		// patch to allow same of multiple account
		module.Offset(0x114510).Patch("EB");

		g_pServerAuthentication->m_bAllowDuplicateAccounts = true;
	}
}
