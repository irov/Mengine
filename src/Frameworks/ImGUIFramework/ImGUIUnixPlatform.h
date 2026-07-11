#pragma once

#include "Environment/Unix/UnixIncluder.h"

bool MengineImGUIUnixPlatform_Init( Display * _display, Window _window );
void MengineImGUIUnixPlatform_Shutdown();
bool MengineImGUIUnixPlatform_IsInitialized();
void MengineImGUIUnixPlatform_NewFrame();
bool MengineImGUIUnixPlatform_ProcessEvent( const XEvent & _event, const char * _text );
