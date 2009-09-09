/*
 *  main.cpp
 *  Mac
 *
 *  Created by Berserk on 22.10.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#include "Config/Config.h"
#include "MacOSApplication.h"

//--------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
	Menge::StringA commandLine;
	for( int i = 1; i < argc; i++ )
	{
		commandLine += Menge::StringA(argv[i]) + Menge::StringA(" ");
	}

	Menge::MacOSApplication macOSApplication( commandLine );
	if( macOSApplication.start() == false )
	{
		return 0;
	}
	macOSApplication.loop();

	return 0;
}
