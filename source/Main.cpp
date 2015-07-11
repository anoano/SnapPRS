/////////////////////////////////////////////////////////////
// CINEMA 4D SDK                                           //
/////////////////////////////////////////////////////////////
// (c) 1989-2004 MAXON Computer GmbH, all rights reserved  //
/////////////////////////////////////////////////////////////

// Starts the plugin registration
#define _CRT_SECURE_NO_DEPRECATE 1  /* VisualC++2005 Ç≈ÇÃåxçêó}êß */
#include "c4d.h"

Bool RegisterSnapAlign();
Bool RegisterSRotate();
Bool RegisterSScale();
Bool RegisterSerialHook();
void FreeSNHook();
Bool IsSerialOKHook();

/////////////////////////////////////////////////////////

C4D_CrashHandler old_handler;

void SDKCrashHandler(Char *crashinfo)
{

	if (old_handler) (*old_handler)(crashinfo);
}


Bool PluginStart(void)
{
	// example of installing a crashhandler
	old_handler = C4DOS.CrashHandler; // backup the original handler (must be called!)
	C4DOS.CrashHandler = SDKCrashHandler; // insert the own handler
	
	
    SYSTEMINFO    stype =        GeGetSystemInfo();
//	Int32 ver =GetC4DVersion();
//	if(ver < 15000) return false;
	if ( stype & (SYSTEMINFO_SAVABLEDEMO | SYSTEMINFO_SAVABLEDEMO_ACTIVE) || stype & SYSTEMINFO_COMMANDLINE)
	{
		if(!RegisterSnapAlign()) return false;
		if(!RegisterSRotate()) return false;
		if(!RegisterSScale()) return false;
	}
	else if (IsNet())
	{
		if (!RegisterSnapAlign()) return false;
		if (!RegisterSRotate()) return false;
		if (!RegisterSScale()) return false;

	}
	else 
	{
		if (!IsSerialOKHook()) return false; //don't load plugins if key is wrong
		if(!RegisterSnapAlign()) return false;
		if(!RegisterSRotate()) return false;
		if(!RegisterSScale()) return false;
	}
	return true;
}

void PluginEnd(void)
{
	FreeSNHook();
}

Bool PluginMessage(Int32 id, void *data)
{
	switch (id)
	{
		case C4DPL_INIT_SYS:
			{
				if (!resource.Init()) return false; // don't start plugin without resource

				// important, the serial hook must be registered before PluginStart(), best in C4DPL_INIT_SYS
				if (!RegisterSerialHook()) return false;
				return true;	
			}

	}

	return false;
}
