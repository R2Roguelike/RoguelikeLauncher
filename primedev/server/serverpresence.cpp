#include "serverpresence.h"
#include "shared/playlist.h"
#include "core/tier0.h"
#include "core/convar/convar.h"

#include <regex>

ConVar* Cvar_hostname;

// Convert a hex digit char to integer.
inline int hctod(char c)
{
	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}
	else
	{
		return c - '0';
	}
}

// This function interprets all 4-hexadecimal-digit unicode codepoint characters like \u4E2D to UTF-8 encoding.
std::string UnescapeUnicode(const std::string& str)
{
	std::string result;

	std::regex r("\\\\u([a-f\\d]{4})", std::regex::icase);
	auto matches_begin = std::sregex_iterator(str.begin(), str.end(), r);
	auto matches_end = std::sregex_iterator();
	std::smatch last_match;

	for (std::sregex_iterator i = matches_begin; i != matches_end; ++i)
	{
		last_match = *i;
		result.append(last_match.prefix());
		unsigned int cp = 0;
		for (int i = 2; i <= 5; ++i)
		{
			cp *= 16;
			cp += hctod(last_match.str()[i]);
		}
		if (cp <= 0x7F)
		{
			result.push_back(cp);
		}
		else if (cp <= 0x7FF)
		{
			result.push_back((cp >> 6) | 0b11000000 & (~(1 << 5)));
			result.push_back(cp & ((1 << 6) - 1) | 0b10000000 & (~(1 << 6)));
		}
		else if (cp <= 0xFFFF)
		{
			result.push_back((cp >> 12) | 0b11100000 & (~(1 << 4)));
			result.push_back((cp >> 6) & ((1 << 6) - 1) | 0b10000000 & (~(1 << 6)));
			result.push_back(cp & ((1 << 6) - 1) | 0b10000000 & (~(1 << 6)));
		}
	}

	if (!last_match.ready())
		return str;
	else
		result.append(last_match.suffix());

	return result;
}

ON_DLL_LOAD_RELIESON("engine.dll", ServerPresence, ConVar, (CModule module))
{
	Cvar_hostname = module.Offset(0x1315BAE8).Deref().RCast<ConVar*>();
}
