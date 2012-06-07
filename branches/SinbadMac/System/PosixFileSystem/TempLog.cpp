//
//  TempLog.cpp
//  Mengine
//
//  Created by Dgut on 28.01.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdarg.h>
#include <stdlib.h>
#include "Application.h"

void MAGIC_LOG( const char * text, ... )
{
    static char buffer[ 4096 ];
    
    va_list	arg;
	
	va_start( arg, text );
	vsprintf( buffer, text, arg );
	va_end( arg );
    
    Menge::Holder< Menge::Application >::hostage()->logMessage( buffer, Menge::LM_LOG );
}