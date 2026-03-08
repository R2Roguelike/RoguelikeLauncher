
ON_DLL_LOAD("server.dll", MiscServerFixes, (CModule module))
{
	// nop out call to VGUI shutdown since it crashes the game when quitting from the console
	module.Offset(0x154A96).NOP(5);

	// allow 3 main weapons in singleplayer :3
	// this breaks multiplayer but who gaf
	module.Offset(0xE1675).Patch("04");
}
