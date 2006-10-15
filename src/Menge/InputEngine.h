// ***************************************************************
//  InputEngine.h   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

#	include "DllModule.h"
#	include "Keeper.h"

#	include <string>
#	include <map>

class InputSystemInterface;

namespace Menge
{
	class InputScriptFunctionMap;
	class InputManager;
	class ScriptEngine;

	class InputEngine
		: public DllModuleInterface<InputSystemInterface>
	{
	public:
		InputEngine(const std::string &_dllModule);
		~InputEngine();

	public:
		bool Init();

	public:
		bool LoadFunctionMap(const std::string &File);
		InputScriptFunctionMap * GetFunctionMap(const std::string &Name);
		bool SetActiveFunctionMap(const std::string &Name, bool Value);

	public:
		bool IsKey(unsigned int index,int key_state)const;
		bool IsAnyKey()const;
		bool GetKey(char *ch,int key_state)const;

		//mouse
		void SetPositionAndSpeed (int X, int Y, int Z, int Speed);
		void SetRange (int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		void SetPosition (int axis, int value);

		int GetPosition (int axis)const;
		int GetDelta	(int axis)const;

		bool IsMove()const;
		bool IsAnyButton()const;
		bool IsButton(int but,int but_state)const;

	public:
		void Update();

	private:
		typedef std::map<std::string, InputScriptFunctionMap *> TMapInputScriptFunctionMap;

		TMapInputScriptFunctionMap m_MapInputScriptFunctionMap;
	};
}