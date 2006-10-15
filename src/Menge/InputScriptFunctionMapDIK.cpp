// ***************************************************************
//  InputScriptFunctionMapDIK.cpp   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputScriptFunctionMap.h"
#	include "InputEnum.h"

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::InitDIK()
	{
		/****************************************************************************
		*
		* DirectInput keyboard scan codes
		*
		****************************************************************************/
		m_MapDIK["DIK_ESCAPE"] = 0x01;
		m_MapDIK["DIK_1"] = DIK_1;
		m_MapDIK["DIK_2"] =	0x03;
		m_MapDIK["DIK_3"] = 0x04;
		m_MapDIK["DIK_4"] = 0x05;
		m_MapDIK["DIK_5"] = 0x06;
		m_MapDIK["DIK_6"] = 0x07;
		m_MapDIK["DIK_7"] = 0x08;
		m_MapDIK["DIK_8"] = 0x09;
		m_MapDIK["DIK_9"] = 0x0A;
		m_MapDIK["DIK_0"] = 0x0B;
		m_MapDIK["DIK_MINUS"] = 0x0C; /* - on main keyboard */
		m_MapDIK["DIK_EQUALS"] = 0x0D;
		m_MapDIK["DIK_BACK"] = 0x0E; /* backspace */
		m_MapDIK["DIK_TAB"] = 0x0F;
		m_MapDIK["DIK_Q"] = 0x10;
		m_MapDIK["DIK_W"] = 0x11;
		m_MapDIK["DIK_E"] = 0x12;
		m_MapDIK["DIK_R"] = 0x13;
		m_MapDIK["DIK_T"] = 0x14;
		m_MapDIK["DIK_Y"] = 0x15;
		m_MapDIK["DIK_U"] = 0x16;
		m_MapDIK["DIK_I"] = 0x17;
		m_MapDIK["DIK_O"] = 0x18;
		m_MapDIK["DIK_P"] = 0x19;
		m_MapDIK["DIK_LBRACKET"] = 0x1A;
		m_MapDIK["DIK_RBRACKET"] = 0x1B;
		m_MapDIK["DIK_RETURN"] = 0x1C; /* Enter on main keyboard */
		m_MapDIK["DIK_LCONTROL"] = 0x1D;
		m_MapDIK["DIK_A"] = 0x1E;
		m_MapDIK["DIK_S"] = 0x1F;
		m_MapDIK["DIK_D"] = 0x20;
		m_MapDIK["DIK_F"] = 0x21;
		m_MapDIK["DIK_G"] = 0x22;
		m_MapDIK["DIK_H"] = 0x23;
		m_MapDIK["DIK_J"] = 0x24;
		m_MapDIK["DIK_K"] = 0x25;
		m_MapDIK["DIK_L"] = 0x26;
		m_MapDIK["DIK_SEMICOLON"] = 0x27;
		m_MapDIK["DIK_APOSTROPHE"] = 0x28;
		m_MapDIK["DIK_GRAVE"] = 0x29; /* accent grave */
		m_MapDIK["DIK_LSHIFT"] = 0x2A;
		m_MapDIK["DIK_BACKSLASH"] = 0x2B;
		m_MapDIK["DIK_Z"] = 0x2C;
		m_MapDIK["DIK_X"] = 0x2D;
		m_MapDIK["DIK_C"] = 0x2E;
		m_MapDIK["DIK_V"] = 0x2F;
		m_MapDIK["DIK_B"] = 0x30;
		m_MapDIK["DIK_N"] = 0x31;
		m_MapDIK["DIK_M"] = 0x32;
		m_MapDIK["DIK_COMMA"] = 0x33;
		m_MapDIK["DIK_PERIOD"] = 0x34; /* . on main keyboard */
		m_MapDIK["DIK_SLASH"] = 0x35; /* / on main keyboard */
		m_MapDIK["DIK_RSHIFT"] = 0x36;
		m_MapDIK["DIK_MULTIPLY"] = 0x37; /* * on numeric keypad */
		m_MapDIK["DIK_LMENU"] = 0x38; /* left Alt */
		m_MapDIK["DIK_SPACE"] = 0x39;
		m_MapDIK["DIK_CAPITAL"] = 0x3A;
		m_MapDIK["DIK_F1"] = 0x3B;
		m_MapDIK["DIK_F2"] = 0x3C;
		m_MapDIK["DIK_F3"] = 0x3D;
		m_MapDIK["DIK_F4"] = 0x3E;
		m_MapDIK["DIK_F5"] = 0x3F;
		m_MapDIK["DIK_F6"] = 0x40;
		m_MapDIK["DIK_F7"] = 0x41;
		m_MapDIK["DIK_F8"] = 0x42;
		m_MapDIK["DIK_F9"] = 0x43;
		m_MapDIK["DIK_F10"] = 0x44;
		m_MapDIK["DIK_NUMLOCK"] = 0x45;
		m_MapDIK["DIK_SCROLL"] = 0x46; /* Scroll Lock */
		m_MapDIK["DIK_NUMPAD7"] = 0x47;
		m_MapDIK["DIK_NUMPAD8"] = 0x48;
		m_MapDIK["DIK_NUMPAD9"] = 0x49;
		m_MapDIK["DIK_SUBTRACT"] = 0x4A; /* - on numeric keypad */
		m_MapDIK["DIK_NUMPAD4"] = 0x4B;
		m_MapDIK["DIK_NUMPAD5"] = 0x4C;
		m_MapDIK["DIK_NUMPAD6"] = 0x4D;
		m_MapDIK["DIK_ADD"] = 0x4E; /* + on numeric keypad */
		m_MapDIK["DIK_NUMPAD1"] = 0x4F;
		m_MapDIK["DIK_NUMPAD2"] = 0x50;
		m_MapDIK["DIK_NUMPAD3"] = 0x51;
		m_MapDIK["DIK_NUMPAD0"] = 0x52;
		m_MapDIK["DIK_DECIMAL"] = 0x53; /* . on numeric keypad */
		m_MapDIK["DIK_OEM_102"] = 0x56; /* <> or \| on RT 102-key keyboard (Non-U.S.) */
		m_MapDIK["DIK_F11"] = 0x57;
		m_MapDIK["DIK_F12"] = 0x58;
		m_MapDIK["DIK_F13"] = 0x64; /* (NEC PC98) */
		m_MapDIK["DIK_F14"] = 0x65; /* (NEC PC98) */
		m_MapDIK["DIK_F15"] = 0x66; /* (NEC PC98) */
		m_MapDIK["DIK_KANA"] = 0x70; /* (Japanese keyboard) */
		m_MapDIK["DIK_ABNT_C1"] = 0x73; /* /? on Brazilian keyboard */
		m_MapDIK["DIK_CONVERT"] = 0x79; /* (Japanese keyboard) */
		m_MapDIK["DIK_NOCONVERT"] = 0x7B; /* (Japanese keyboard) */
		m_MapDIK["DIK_YEN"] = 0x7D; /* (Japanese keyboard) */
		m_MapDIK["DIK_ABNT_C2"] = 0x7E; /* Numpad . on Brazilian keyboard */
		m_MapDIK["DIK_NUMPADEQUALS"] = 0x8D; /* = on numeric keypad (NEC PC98) */
		m_MapDIK["DIK_PREVTRACK"] = 0x90; /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
		m_MapDIK["DIK_AT"] = 0x91; /* (NEC PC98) */
		m_MapDIK["DIK_COLON"] = 0x92; /* (NEC PC98) */
		m_MapDIK["DIK_UNDERLINE"] = 0x93; /* (NEC PC98) */
		m_MapDIK["DIK_KANJI"] = 0x94; /* (Japanese keyboard) */
		m_MapDIK["DIK_STOP"] = 0x95; /* (NEC PC98) */
		m_MapDIK["DIK_AX"] = 0x96; /* (Japan AX) */
		m_MapDIK["DIK_UNLABELED"] = 0x97; /* (J3100) */
		m_MapDIK["DIK_NEXTTRACK"] = 0x99; /* Next Track */
		m_MapDIK["DIK_NUMPADENTER"] = 0x9C; /* Enter on numeric keypad */
		m_MapDIK["DIK_RCONTROL"] = 0x9D;
		m_MapDIK["DIK_MUTE"] = 0xA0; /* Mute */
		m_MapDIK["DIK_CALCULATOR"] = 0xA1; /* Calculator */
		m_MapDIK["DIK_PLAYPAUSE"] = 0xA2; /* Play / Pause */
		m_MapDIK["DIK_MEDIASTOP"] = 0xA4; /* Media Stop */
		m_MapDIK["DIK_VOLUMEDOWN"] = 0xAE; /* Volume - */
		m_MapDIK["DIK_VOLUMEUP"] = 0xB0; /* Volume + */
		m_MapDIK["DIK_WEBHOME"] = 0xB2; /* Web home */
		m_MapDIK["DIK_NUMPADCOMMA"] = 0xB3; /* , on numeric keypad (NEC PC98) */
		m_MapDIK["DIK_DIVIDE"] = 0xB5; /* / on numeric keypad */
		m_MapDIK["DIK_SYSRQ"] = 0xB7;
		m_MapDIK["DIK_RMENU"] = 0xB8; /* right Alt */
		m_MapDIK["DIK_PAUSE"] = 0xC5; /* Pause */
		m_MapDIK["DIK_HOME"] = 0xC7; /* Home on arrow keypad */
		m_MapDIK["DIK_UP"] = 0xC8; /* UpArrow on arrow keypad */
		m_MapDIK["DIK_PRIOR"] = 0xC9; /* PgUp on arrow keypad */
		m_MapDIK["DIK_LEFT"] = 0xCB; /* LeftArrow on arrow keypad */
		m_MapDIK["DIK_RIGHT"] = 0xCD; /* RightArrow on arrow keypad */
		m_MapDIK["DIK_END"] = 0xCF; /* End on arrow keypad */
		m_MapDIK["DIK_DOWN"] = 0xD0; /* DownArrow on arrow keypad */
		m_MapDIK["DIK_NEXT"] = 0xD1; /* PgDn on arrow keypad */
		m_MapDIK["DIK_INSERT"] = 0xD2; /* Insert on arrow keypad */
		m_MapDIK["DIK_DELETE"] = 0xD3; /* Delete on arrow keypad */
		m_MapDIK["DIK_LWIN"] = 0xDB; /* Left Windows key */
		m_MapDIK["DIK_RWIN"] = 0xDC; /* Right Windows key */
		m_MapDIK["DIK_APPS"] = 0xDD; /* AppMenu key */
		m_MapDIK["DIK_POWER"] = 0xDE; /* System Power */
		m_MapDIK["DIK_SLEEP"] = 0xDF; /* System Sleep */
		m_MapDIK["DIK_WAKE"] = 0xE3; /* System Wake */
		m_MapDIK["DIK_WEBSEARCH"] = 0xE5; /* Web Search */
		m_MapDIK["DIK_WEBFAVORITES"] = 0xE6; /* Web Favorites */
		m_MapDIK["DIK_WEBREFRESH"] = 0xE7; /* Web Refresh */
		m_MapDIK["DIK_WEBSTOP"] = 0xE8; /* Web Stop */
		m_MapDIK["DIK_WEBFORWARD"] = 0xE9; /* Web Forward */
		m_MapDIK["DIK_WEBBACK"] = 0xEA; /* Web Back */
		m_MapDIK["DIK_MYCOMPUTER"] = 0xEB; /* My Computer */
		m_MapDIK["DIK_MAIL"] = 0xEC; /* Mail */
		m_MapDIK["DIK_MEDIASELECT"] = 0xED; /* Media Select */

		/*
		* Alternate names for keys, to facilitate transition from DOS.
		*/
		m_MapDIK["DIK_BACKSPACE"] = DIK_BACK; /* backspace */
		m_MapDIK["DIK_NUMPADSTAR"] =  DIK_MULTIPLY; /* * on numeric keypad */
		m_MapDIK["DIK_LALT"] =  DIK_LMENU; /* left Alt */
		m_MapDIK["DIK_CAPSLOCK"] =  DIK_CAPITAL; /* CapsLock */
		m_MapDIK["DIK_NUMPADMINUS"] =  DIK_SUBTRACT; /* - on numeric keypad */
		m_MapDIK["DIK_NUMPADPLUS"] =  DIK_ADD; /* + on numeric keypad */
		m_MapDIK["DIK_NUMPADPERIOD"] =  DIK_DECIMAL; /* . on numeric keypad */
		m_MapDIK["DIK_NUMPADSLASH"] =  DIK_DIVIDE;/* / on numeric keypad */
		m_MapDIK["DIK_RALT"] =  DIK_RMENU; /* right Alt */
		m_MapDIK["DIK_UPARROW"] =  DIK_UP; /* UpArrow on arrow keypad */
		m_MapDIK["DIK_PGUP"] =  DIK_PRIOR; /* PgUp on arrow keypad */
		m_MapDIK["DIK_LEFTARROW"] =  DIK_LEFT; /* LeftArrow on arrow keypad */
		m_MapDIK["DIK_RIGHTARROW"] =  DIK_RIGHT; /* RightArrow on arrow keypad */
		m_MapDIK["DIK_DOWNARROW"] =  DIK_DOWN; /* DownArrow on arrow keypad */
		m_MapDIK["DIK_PGDN"] =  DIK_NEXT; /* PgDn on arrow keypad */


		m_MapDIK["DI_DEFAULT"] =  DI_DEFAULT; /* PgDn on arrow keypad */
		m_MapDIK["DI_RELEASED"] =  DI_RELEASED; /* PgDn on arrow keypad */
		m_MapDIK["DI_HELD"] =  DI_HELD; /* PgDn on arrow keypad */
		m_MapDIK["DI_PRESSED"] =  DI_PRESSED; /* PgDn on arrow keypad */
		m_MapDIK["MOUSE_LEFT"] =  MOUSE_LEFT; /* PgDn on arrow keypad */
		m_MapDIK["MOUSE_RIGHT"] =  MOUSE_RIGHT; /* PgDn on arrow keypad */
		m_MapDIK["MOUSE_MIDDLE"] =  MOUSE_MIDDLE; /* PgDn on arrow keypad */
	}
	//////////////////////////////////////////////////////////////////////////
	short int InputScriptFunctionMap::GetDIK(const std::string &Name)
	{
		return m_MapDIK[Name];
	}
}