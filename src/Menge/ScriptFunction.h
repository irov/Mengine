#	pragma once

#	include "LuaScript/LuaScriptFunction.h"

#	include <string>

namespace RvEngine
{
	class ScriptEngine;

	class Node;

	class ScriptFunction
		: public CLuaScriptFunction
	{
	public:
		ScriptFunction(ScriptEngine *_scriptEngine);
		ScriptFunction(ScriptEngine *_scriptEngine, const std::string &_function);

	public:
		void callFunctionVoid();
		int callFunctionInt();
		bool callFunctionBool();

		void callFunctionVoidFloat(float _value);
		void callFunctionVoidFloatFloat(float _value1, float _value2);

	public:
		template<class T_Ret>
		T_Ret callFunctionObject(Node *_node);

		void callFunctionVoidObject(Node *_node);
		int callFunctionIntObject(Node *_node);
		bool callFunctionBoolObject(Node *_node);

		template<class T_Ret, class T_Value>
		T_Ret callFunctionObjectValue(Node *_node, T_Value _value);

		void callFunctionVoidObjectFloat(Node *_node, float _value);
		int callFunctionIntObjectFloat(Node *_node, float _value);

		void callFunctionVoidObjectString(Node *_node, const std::string & _value);
		int callFunctionIntObjectString(Node *_node, const std::string & _value);

		void callFunctionVoidObjectInt(Node *_node, int _value);
		int callFunctionIntObjectInt(Node *_node, int _value);

		template<class T_Ret, class T_Value_First, class T_Value_Second>
		T_Ret callFunctionObjectValueValue(Node *_node, T_Value_First _value1, T_Value_Second _value2);

		void callFunctionVoidObjectFloatFloat(Node *_node, float _value1, float _value2);
		int callFunctionIntObjectFloatFloat(Node *_node, float _value1, float _value2);

		template<class T_Ret>
		T_Ret callFunctionObjectObject(Node *_node1,Node *_node2);

		void callFunctionVoidObjectObject(Node *_node1,Node *_node2);
		int callFunctionIntObjectObject(Node *_node1,Node *_node2);


		template<class T_Ret, class T_Value>
		T_Ret callFunctionObjectObjectValue(Node *_node1,Node *_node2, T_Value _value);

		void callFunctionVoidObjectObjectFloat(Node *_node1,Node *_node2, float _value);
		int callFunctionIntObjectObjectFloat(Node *_node1,Node *_node2, float _value);

		void callFunctionVoidObjectObjectInt(Node *_node1,Node *_node2, int _value);
		int callFunctionIntObjectObjectInt(Node *_node1,Node *_node2, int _value);
	};
}