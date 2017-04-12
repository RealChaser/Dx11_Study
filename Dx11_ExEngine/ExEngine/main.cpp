#include "System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* pSystem;
	bool result;

	// Create the system object.
	pSystem = new System;
	if(!pSystem)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = pSystem->Initialize();
	if(result)
	{
		pSystem->Run();
	}

	// Shutdown and release the system object.
	pSystem->Shutdown();
	delete pSystem;
	pSystem = 0;

	return 0;
}