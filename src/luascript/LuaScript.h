/*(c)IROV*/

#	if !BOOST_PP_IS_ITERATING

#	ifndef LUASCRIPT_H
#	define LUASCRIPT_H

#	include "LuaScriptInclude.h"

#	include <string>

class CLuaScript
{
public:
	enum
	{
		TYPE_INTEGER = LUA_TNUMBER,
		TYPE_STRING = LUA_TSTRING,
		TYPE_FUNCTION = LUA_TFUNCTION
	};

public:
	CLuaScript();
	~CLuaScript();

public:
	void Init();
	void Close();
	
	int DoFile(const std::string &file);
	int DoBuffer(const char *buff, size_t sz);
	int DoBuffer(const std::vector<char> &buff);
	int DoString(const std::string &str);

	inline TLuaState GetLuaState(){return m_LuaState;}
	
	inline TLuaObject &GetGlobals(){return m_LuaGlobalVar;}
	inline const TLuaObject &GetGlobals()const{return m_LuaGlobalVar;}

	inline const std::string & GetError(){return m_Error;} // new 21.10

	inline TLuaClass DefineClass(){return luabind::module(m_LuaState);}

	TLuaObject GetLuaObject(const std::string &Name)const;
	bool IsLuaObject(const std::string &Name);

	TLuaObject GenObject(const std::string &Type);

	template<class T>
	TLuaObject * GenLuaObjectT( T *Object)
	{
		return new TLuaObject(m_LuaState,Object);
	}

	//TLuaObject Eval(const std::string &Expression);

	//template <typename T> inline
	//	T EvalT(const std::string &Expression)
	//{
	//	luabind::object o = Eval(Expression);
	//	return luabind::object_cast<T>(o);
	//}

	template<class T>
		inline T GetCppObject(const std::string &Name)
	{
		return luabind::object_cast<T>(m_LuaGlobalVar[Name]);
	}

	template<class T>
		inline static T GetCppObject(const TLuaObject &LuaObject)
	{
		return luabind::object_cast<T>(LuaObject);
	}



	template<class T>
	inline void SetLuaObject(const std::string &Name, const T &Value)
	{
		m_LuaGlobalVar[Name] = Value;
	}

	template<class T>
	inline void SetLuaObject(const std::string &Name, const TLuaObject &LuaObject)
	{
		m_LuaGlobalVar[Name] = LuaObject;
	}

	void SetLuaObject(const std::string &Name, const std::string &Value);

	template<class T>
		inline static void SetLuaObject(const TLuaObject &LuaObject, const T &Value)
	{
		LuaObject = Value;
	}

	template<class T>
	inline void SetLuaMemberObjectClass(const std::string &Class, const std::string &Member, const T &Value)
	{
		TLuaObject ObjectClass = GetLuaObject(Class);

		TLuaObject ObjectMember = ObjectClass.at(Member);

		ObjectMember = Value;
	}

	template<class F>
		inline void DefineFunction(const std::string &Name,F Ptr)
	{
		luabind::module(m_LuaState)
			[
				luabind::def(Name.c_str(),Ptr)
			];
	}

	template<class F,class P>
		inline void DefineFunction(const std::string &Name,F Ptr,const P &Policies)
	{
		luabind::module(m_LuaState)
			[
				luabind::def(Name.c_str(),fun_ptr,fun_policies)
			];	
	}


#define BOOST_PP_ITERATION_PARAMS_1 (4, (0, 4, "luascript.h", 1))
#include BOOST_PP_ITERATE()

private:
	TLuaState	m_LuaState;
	TLuaObject	m_LuaGlobalVar;
	std::string m_Error;

};

#endif


#elif BOOST_PP_ITERATION_FLAGS() == 1

template<class R BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),class A)>
R CallFunction(const std::string &Name BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const A, &a ) )
{
	return static_cast<R>(
		luabind::call_function<R>(m_LuaState, Name.c_str() BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),a) )
		);
}

template<class R BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),class A)>
static R CallFunction(TLuaObject const& LuaObject BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const A, &a ) )
{
	return static_cast<R>(
		luabind::call_function<R>(LuaObject BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),a) )
		);
}

template<class R BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),class A)>
static R CallMember(TLuaObject const& LuaObject, const std::string &Name BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const A, &a ) )
{
	return static_cast<R>(
		luabind::call_member<R>(LuaObject, Name.c_str() BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),a) )
		);
}

#endif
