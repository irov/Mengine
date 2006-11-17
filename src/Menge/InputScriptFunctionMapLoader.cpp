// ***************************************************************
//  InputScriptFunctionMapLoader.cpp   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputScriptFunctionMap.h"

#	include "InputScriptFunctionCommand.h"

#	include "InputEngine.h"

#	include "XmlReader.h"

namespace Menge
{
	void InputScriptFunctionMap::Loader(TiXmlElement *Tree)
	{
		XML_FOR_EACH_TREE(Tree)
		{
			//<DELTHA_MOUSE FUNCTION = "UserPlayerChangeDelthaMouse" />
			XML_CHECK_NODE("DELTHA_MOUSE")
			{
				XML_DEF_ATTRIBUTES_NODE(FUNCTION);

				AddChangeDelthaMouse(FUNCTION);
			}

			XML_CHECK_NODE("WHELL_MOUSE")
			{
				XML_DEF_ATTRIBUTES_NODE(FUNCTION);

				AddChangeWhellMouse(FUNCTION);
			}

			XML_CHECK_NODE("CONTROL")
			{
				XML_DEF_ATTRIBUTES_NODE(NAME);

				InputScriptFunctionCommand * FunctionCommand = 
					CreateInputControls(NAME);

				XML_DEF_ATTRIBUTES_NODE(ACTIVE);

				if( ACTIVE == "TRUE" )
				{
					FunctionCommand->Active();
				}
				else 
				{
					FunctionCommand->Deactive();
				}

				XML_DEF_ATTRIBUTES_NODE(FUNCTION);

				if( FUNCTION.empty() == false )
				{
					FunctionCommand->BindFunction(FUNCTION);
				}

				XML_FOR_EACH()
				{
					//	<KEY TYPE = "DIK_2" STATE = "DI_PRESSED" />
					XML_CHECK_NODE("KEY")
					{
						XML_DEF_ATTRIBUTES_NODE(TYPE);
						XML_DEF_ATTRIBUTES_NODE(STATE);

						short int Key = GetDIK(TYPE);
						short int State = GetDIK(STATE);
						FunctionCommand->AddKey(Key,State);
					}

					//<BUTTON TYPE = "MOUSE_RIGHT" STATE = "DI_PRESSED" />
					XML_CHECK_NODE("BUTTON")
					{
						XML_DEF_ATTRIBUTES_NODE(TYPE);
						XML_DEF_ATTRIBUTES_NODE(STATE);

						short int Button = GetDIK(TYPE);
						short int State = GetDIK(STATE);
						FunctionCommand->AddMouseButton(Button,State);
					}
				}            
			}
		}
	}
}