#	pragma once

#	include <map>

#	include <algorithm>

namespace Utility
{
	//////////////////////////////////////////////////////////////////////////
	template<class T_Function , class T_Caster >
	class FMapSecondBinder
	{
	public:
		FMapSecondBinder(T_Function &Function)
			: m_Function(Function)
		{
		}

	public:
		template<
			typename T_Key, 
			typename T_Value> inline
			void operator()( const std::pair<const T_Key, T_Value> &Pair )const
		{
			m_Function.operator()(static_cast<T_Caster>(Pair.second));
		}

		template<
			typename T_Key, 
			typename T_Value> inline
			void operator()( std::pair<const T_Key, T_Value> &Pair )const
		{
			m_Function.operator()(static_cast<T_Caster>(Pair.second));
		}

	private:
		T_Function & m_Function;
	};
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Function, 
		typename T_Caster > inline
		FMapSecondBinder<T_Function,T_Caster> map_second_binder(T_Function &Function)
	{
		return FMapSecondBinder<T_Function,T_Caster>(Function);
	}
	//////////////////////////////////////////////////////////////////////////
	// MAP_FOR_EACH_SECOND_HELPER
	//////////////////////////////////////////////////////////////////////////
	template<class T_Caster>
	class FMapForEachSecondHelper
	{
	public:
		template<
			typename T_Iterator, 
			typename T_Function> 
			static inline void for_each(T_Iterator First, T_Iterator Last, T_Function &Func)
		{
			std::for_each(
				First,
				Last,
				map_second_binder<T_Function,T_Caster>(Func));
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// MAP_FOR_EACH_SECOND
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Iterator,
		typename T_Function> inline
		void map_for_each_second(T_Iterator First, T_Iterator Last, T_Function &Func)
	{
		typedef T_Iterator::value_type::second_type T_Caster;

		FMapForEachSecondHelper<T_Caster>::for_each(
			First,Last,Func
			);
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Key, typename T_Value, 
		typename T_Result,
		typename T_Class> inline
		void map_for_each_second( std::map<T_Key,T_Value> &Container, T_Result (T_Class::*Function)() )
	{
		FMapForEachSecondHelper<T_Class *>::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Key, typename T_Value, 
		typename T_Result,
		typename T_Class>
		void map_for_each_second( const std::map<T_Key,T_Value> &Container, T_Result (T_Class::*Function)() const )
	{
		FMapForEachSecondHelper<T_Class *>::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Key, typename T_Value, 
		typename T_Result,
		typename T_Class,
		typename T_ClassArg,
		typename T_Arg>
		void map_for_each_second( std::map<T_Key,T_Value> &Container, T_Result (T_Class::*Function)(T_ClassArg) , const T_Arg &Arg )
	{
		FMapForEachSecondHelper<T_Class *>::for_each(
			Container.begin(),
			Container.end(),
			std::bind2nd(std::mem_fun(Function),Arg));
	}
	//////////////////////////////////////////////////////////////////////////
	// MAP_FOR_EACH_FIRST
	//////////////////////////////////////////////////////////////////////////
	template<class T_Iterator, class T_Function> inline
		void map_for_each_first(T_Iterator First, T_Iterator Last, T_Function Func)
	{
		for (; First != Last; ++First)
			Func(First->first);
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Key, typename T_Value> inline
		void map_erase_ptr(std::map<T_Key,T_Value> &Container, const T_Key &Key)
	{
		typename std::map<T_Key,T_Value>::iterator it_find =
			Container.find(Key);

		if( it_find != Container.end() )
		{
			delete it_find->second;
			Container.erase(it_find);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Key, typename T_Value> inline
		void map_erase_ptr(std::map<T_Key,T_Value> &Container, typename std::map<T_Key,T_Value>::iterator It)
	{
		delete It->second;
		Container.erase(It);
	}

	//////////////////////////////////////////////////////////////////////////
	template<typename T_Key, typename T_Value> inline
		void map_delete_ptr(std::map<T_Key,T_Value> &Container, typename std::map<T_Key,T_Value>::iterator It)
	{
		delete It->second;
		Container.erase(It);
	}

	template<typename T_Key, typename T_Value, class T_Functor>
	void map_erase_if( std::map<T_Key,T_Value> &Container, T_Functor Functor)
	{
		typedef std::map<T_Key,T_Value> TContainer;
		TContainer::iterator it_end = Container.end();
		TContainer::iterator it = Container.begin();
		while(it != it_end)
		{
			if( Functor(it->second) == false )
			{
				Container.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	template<typename T_Key, typename T_Value>
	class FMapSecondBackInserter
	{
	public:
		typedef std::map<T_Key, T_Value> TContainer;

		FMapSecondBackInserter( TContainer &Container )
			: m_Container(Container)
		{
		}

		FMapSecondBackInserter( const FMapSecondBackInserter &F )
			: m_Container(F.m_Container)
		{
		}

		void operator() ( const T_Value &Value )
		{

		}

		FMapSecondBackInserter& operator = (const T_Value &Value)
		{	// push value into container
			m_Container.push_back(Value.second);
			return (*this);
		}

		FMapSecondBackInserter& operator*()
		{	// pretend to return designated value
			return (*this);
		}

		FMapSecondBackInserter& operator++()
		{	// pretend to preincrement
			return (*this);
		}

		FMapSecondBackInserter operator++(int)
		{	// pretend to postincrement
			return (*this);
		}

	private:
		TContainer &m_Container;
	};

	template<typename T_Key, typename T_Value>
		inline FMapSecondBackInserter<T_Key,T_Value> 
		map_second_back_inserter( std::map<T_Key, T_Value> &Container)
	{
		return FMapSecondBackInserter<T_Key,T_Value>(Container);
	}

	template<
		typename T_Key, typename T_Value, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void map_find_and_execute( 
		std::map<T_Key, T_Value> &Container,
		const T_Key & FindValue, 
		T_Result (T_Class::*Function)(T_Arg)const,
		const T_Arg &Value )
	{
		std::map<T_Key, T_Value>::const_iterator It_find = 
			Container.find(FindValue);

		if( It_find != Container.end() )
		{
			std::bind2nd(std::mem_fun(Function),Value)(It_find->second);
		}
	}

	template<
		typename T_Key, typename T_Value, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void map_find_and_execute( 
		std::map<T_Key, T_Value> &Container,
		const T_Key & FindValue, 
		T_Result (T_Class::*Function)(T_Arg),
		const T_Arg &Value )
	{
		std::map<T_Key, T_Value>::const_iterator It_find = 
			Container.find(FindValue);

		if( It_find != Container.end() )
		{
			std::bind2nd(std::mem_fun(Function),Value)(It_find->second);
		}
	}


	template<
		typename T_Key, typename T_Value, 
		typename T_Function> inline 
		void map_find_and_execute( 
		std::map<T_Key, T_Value> &Container,
		const T_Key & FindValue, 
		T_Function &Function )
	{
		std::map<T_Key, T_Value>::const_iterator It_find = 
			Container.find(FindValue);

		if( It_find != Container.end() )
		{
			Function(It_find->second);
		}
	}

	template<typename T_Key, typename T_Value> inline
		typename std::map<T_Key, T_Value>::iterator map_insert(
		std::map<T_Key, T_Value> &Container,
		const T_Key &Key,
		const T_Value &Value)
	{
		std::map<T_Key, T_Value>::iterator it_find = 
			Container.lower_bound(Key);

		if( it_find != Container.end() )
		{
			if( it_find->first == Key )
			{
				return it_find;
			}
		}

		return Container.insert(it_find,std::make_pair(Key,Value));
	}

	template<typename T_Key, typename T_Value> inline
		typename std::map<T_Key, T_Value>::iterator map_insert_if(
		std::map<T_Key, T_Value> &Container,
		const T_Key &Key,
		const T_Value &Value)
	{
		std::map<T_Key, T_Value>::iterator it_find = 
			Container.lower_bound(Key);

		if( it_find != Container.end() )
		{
			if( it_find->first == Key )
			{
				return Container.end();
			}
		}

		return Container.insert(it_find,std::make_pair(Key,Value));
	}
}