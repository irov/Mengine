#	pragma once

#	include <boost/function.hpp>
#	include <map>

struct CInputFunctionState 
{
	CInputFunctionState( short int Key, short int State )
		:m_Key(Key)
		,m_State(State)
	{}

	CInputFunctionState( int KeyState )
		:m_Value(KeyState)
	{}

	union
	{
		struct
		{
			short int m_Key;
			short int m_State;
		};
		int m_Value;
	};

	int MakeInt()
	{
		return m_Value;
	}
};

struct function_base_wraper{};

class CInputFunctionMapBase
{
public:
	typedef std::map<int,function_base_wraper*> TMapFunction;

public:
	virtual void ExecuteFunction( TMapFunction::const_iterator it_function  ) = 0;

public:
	TMapFunction m_MapFunctionKeyboard;
	TMapFunction m_MapFunctionMouse;
};

template<class T>
class CInputFunctionMapClass
	: public CInputFunctionMapBase
{
	template<class T>
	struct function1_class_wraper
		: function_base_wraper
	{
		typedef boost::function1<void, T*> type;

		function1_class_wraper( const type &Func)
			: m_Func(Func)
		{}

		type m_Func;
	};

public:

	typedef typename function1_class_wraper<T>::type TFunction;


	CInputFunctionMapClass( T *Object)
		: m_Object(Object)
	{
	}

	template<class F>
		void BindKeyboard(int Key, int State,F Func)
	{
		int FunctionState = CInputFunctionState(Key,State).MakeInt();
		m_MapFunctionKeyboard[FunctionState]= new function1_class_wraper<T>(boost::bind(Func,_1));
	}

	template<class F>
		void BindMouse(int Button, int State,F Func)
	{
		int FunctionState = CInputFunctionState(Button,State).MakeInt();
		m_MapFunctionMouse[FunctionState]= new function1_class_wraper<T>(boost::bind(Func,_1));
	}

	void ExecuteFunction( TMapFunction::const_iterator it_function )
	{
		static_cast<function1_class_wraper<T> *>(it_function->second)->m_Func(m_Object);
	}

private:
	T *m_Object;	
};


