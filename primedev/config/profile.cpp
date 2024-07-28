#include "config/profile.h"
#include <string>

std::string GetRoguelikePrefix()
{
	return ROGUELIKE_FOLDER_PREFIX;
}

void InitialiseRoguelikePrefix()
{
	char* clachar = strstr(GetCommandLineA(), "-profile=");
	if (clachar)
	{
		std::string cla = std::string(clachar);
		if (strncmp(cla.substr(9, 1).c_str(), "\"", 1))
		{
			size_t space = cla.find(" ");
			std::string dirname = cla.substr(9, space - 9);
			ROGUELIKE_FOLDER_PREFIX = dirname;
		}
		else
		{
			std::string quote = "\"";
			size_t quote1 = cla.find(quote);
			size_t quote2 = (cla.substr(quote1 + 1)).find(quote);
			std::string dirname = cla.substr(quote1 + 1, quote2);
			ROGUELIKE_FOLDER_PREFIX = dirname;
		}
	}
	else
	{
		ROGUELIKE_FOLDER_PREFIX = "R2Roguelike";
	}

	// set the console title to show the current profile
	// dont do this on dedi as title contains useful information on dedi and setting title breaks it as well
	SetConsoleTitleA((std::string("RoguelikeLauncher | ") + ROGUELIKE_FOLDER_PREFIX).c_str());
}
