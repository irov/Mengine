// ***************************************************************
//  ScriptObjectDeclare.h   version:  1.0   ·  date: 05/10/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "ScriptEngine.h"

#	include "ScriptableCaster.h"
#	include "ScriptableRegistrator.h"

#	define SCRIPTABLE_DECLARE_HELPER( CLASS )\
	namespace RvEngine{\
	TLuaObject * CLASS##LuaObjectTypeCast( Node *Object, CLuaScript *LuaScript);\
	void CLASS##RegistryScriptClass(CLuaScript *LuaScript);\
	volatile static ScriptableCaster::\
	CAuto CLASS##LuaObjectTypeCastRegistration(#CLASS, CLASS##LuaObjectTypeCast);\
	volatile static ScriptableRegistrator::\
	CAuto CLASS##RegistryScriptClassRegistration(#CLASS, CLASS##RegistryScriptClass);\
	bool & CLASS##IsScriptInitialize()\
	{\
	static bool Op = false;\
	return Op;\
	}\
	TLuaObject * CLASS##LuaObjectTypeCast( Node *Object, CLuaScript *LuaScript)\
	{\
	return LuaScript->GenLuaObjectT<CLASS>(static_cast<CLASS *>(Object));\
	}\
	void CLASS##RegistryScriptClass(CLuaScript *LuaScript)\
	{\
	if( CLASS##IsScriptInitialize() == false )\
		{\
		CLASS##IsScriptInitialize() = true;

#	define SCRIPTABLE_BASE_REGISTRATION( BASE )\
	NScriptObjectFactory::Implement(#BASE,LuaScript);

#	define SCRIPTABLE_DECLARE_B( CLASS )\
	SCRIPTABLE_DECLARE_HELPER(CLASS)\
	typedef CLASS TDefClass;\
	LuaScript->DefineClass()[\
	luabind::class_<\
	CLASS\
	>(#CLASS)

#	define SCRIPTABLE_DECLARE_A( CLASS , BASE )\
	SCRIPTABLE_DECLARE_HELPER(CLASS)\
	SCRIPTABLE_BASE_REGISTRATION(BASE)\
	typedef CLASS TDefClass;\
	LuaScript->DefineClass()[\
	luabind::class_<\
	CLASS,\
	BASE\
	>(#CLASS)

#	define SCRIPTABLE_DECLARE( CLASS , BASE )\
	SCRIPTABLE_DECLARE_HELPER(CLASS)\
	SCRIPTABLE_BASE_REGISTRATION(BASE)\
	typedef CLASS TDefClass;\
	LuaScript->DefineClass()[\
	luabind::class_<\
	CLASS,\
	BASE\
	>(#CLASS)\
	.def(luabind::constructor<>())

#	define DEF_FUNCTION( FUN )\
	.def(#FUN,&TDefClass::FUN)

#	define DEF_RENAME_FUNCTION( NAME, FUN )\
	.def(NAME,&TDefClass::FUN)

#	define DEF_GET_OBJECT_FUNCTION( FUN )\
	.def_get_object(#FUN,&TDefClass::FUN)

#	define DEF_RENAME_GET_OBJECT_FUNCTION( NAME, FUN )\
	.def_get_object(NAME,&TDefClass::FUN)


#	define SCRIPTABLE_END() ];}}}