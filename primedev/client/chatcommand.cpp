#include "core/convar/convar.h"
#include "core/convar/concommand.h"
#include "localchatwriter.h"

// note: isIngameChat is an int64 because the whole register the arg is stored in needs to be 0'd out to work
// if isIngameChat is false, we use network chat instead
void(__fastcall* ClientSayText)(void* a1, const char* message, uint64_t isIngameChat, bool isTeamChat);

void ConCommand_say(const CCommand& args)
{
	if (args.ArgC() >= 2)
		ClientSayText(nullptr, args.ArgS(), true, false);
}

void ConCommand_say_team(const CCommand& args)
{
	if (args.ArgC() >= 2)
		ClientSayText(nullptr, args.ArgS(), true, true);
}

void ConCommand_log(const CCommand& args)
{
	if (args.ArgC() >= 2)
	{
		LocalChatWriter(LocalChatWriter::GameContext).WriteLine(args.ArgS());
	}
}

void ConCommand_callstack(const CCommand& args)
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
}

ON_DLL_LOAD_CLIENT_RELIESON("engine.dll", ClientChatCommand, ConCommand, (CModule module))
{
	ClientSayText =
		module.Offset(0x54780).RCast<void(__fastcall*)(void* a1, const char* message, uint64_t isIngameChat, bool isTeamChat)>();
	RegisterConCommand("say", ConCommand_say, "Enters a message in public chat", FCVAR_CLIENTDLL);
	RegisterConCommand("say_team", ConCommand_say_team, "Enters a message in team chat", FCVAR_CLIENTDLL);
	RegisterConCommand("log", ConCommand_log, "Log a message to the local chat window", FCVAR_CLIENTDLL);
	RegisterConCommand("callstack", ConCommand_callstack, "Dump callstack", FCVAR_CLIENTDLL);
}
