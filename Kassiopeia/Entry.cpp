#include "Kassiopeia.h"

#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	Kassiopeia::Kassiopeia k{};
	k.run();
	return 0;
}
