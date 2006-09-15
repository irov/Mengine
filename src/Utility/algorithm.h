// ***************************************************************
//  algorithm.h   version:  1.0   ·  date: 05/30/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

#	include <functional>
#	include <algorithm>

namespace Utility
{
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Class, typename T_Member>
	class FCompareMember 
		: public std::unary_function<T_Class, bool>
	{
		typedef T_Member T_Class::* TMemberPtr;
	public:
		inline FCompareMember(TMemberPtr MemPtr, const T_Member& MemVal) 
			: m_MemPtr(MemPtr)
			, m_MemVal(MemVal) 
		{
		}

		inline FCompareMember( const FCompareMember &F) 
			: m_MemPtr(F.m_MemPtr)
			, m_MemVal(F.m_MemVal) 
		{
		}

		template<typename ChainedPtr> inline
			bool operator()(const ChainedPtr& x)const
		{
			return operator()(*x);
		}

		inline bool operator()(const T_Class& obj) const
		{
			return obj.*m_MemPtr == m_MemVal;
		}

	private:
		TMemberPtr			m_MemPtr;
		const T_Member	&	m_MemVal;
	};
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Class, typename T_Member> inline 
		FCompareMember<T_Class,T_Member> 
		compare_mem( T_Member T_Class::* MemPtr, const T_Member& MemVal)
	{
		return FCompareMember<T_Class,T_Member>(MemPtr,MemVal);
	}
	//////////////////////////////////////////////////////////////////////////
	class delete_ptr
	{
	public:
		template<class T> inline
			void operator()( T *Object)const
		{
			delete Object;
		}

		template<class T1, class T2> inline
			void operator()(const std::pair<const T1,T2> &Pair)const
		{
			delete Pair.second;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Contanier, typename T_Function> inline
		void erase_container( T_Contanier & Container, T_Function &Function )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			Function);

		Container.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Contanier> inline 
		void erase_ptr_container( T_Contanier & Container)
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			delete_ptr());

		Container.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Contanier, typename T_Function> inline
		void for_each( T_Contanier &Container, T_Function &Function)
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			Function);
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename T_Contanier, typename T_Function> inline
		typename T_Contanier::iterator find_if( T_Contanier &Container, T_Function &Function)
	{
		return std::find_if(
			Container.begin(),
			Container.end(),
			Function);
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class> inline
		void for_each( T_Contanier &Container, T_Result (T_Class::*Function)() )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class> inline
		void for_each( T_Contanier &Container, T_Result (T_Class::*Function)() const )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void for_each( T_Contanier &Container, T_Result (T_Class::*Function)(T_Arg) const,const T_Arg &Value )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::bind2nd(std::mem_fun(Function),Value));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Class_Arg,
		typename T_Arg> inline 
		void for_each( T_Contanier &Container, T_Result (T_Class::*Function)(T_Class_Arg),const T_Arg &Value )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::bind2nd(std::mem_fun(Function),Value));
	}
	//////////////////////////////////////////////////////////////////////////
	template<
		typename T_Contanier> inline
		void copy_container( const T_Contanier &In, T_Contanier &To)
	{
		std::copy(
			In.begin(),
			In.end(),
			std::back_inserter(To)
			);
	}
	//////////////////////////////////////////////////////////////////////////
	template< 
		typename T_Contanier,
		typename T_Function> inline
		void container_erase_if( T_Contanier &Container, T_Function &Function)
	{
		Container.erase(
			std::remove_if(
			Container.begin(),
			Container.end(),
			Function),
			Container.end());
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T_Contanier> inline
		void push_back_unique( T_Contanier &Container, typename T_Contanier::value_type &Value)
	{
		typename T_Contanier::iterator it = std::find(Container.begin(),Container.end(),Value);
		if( it == Container.end () )
		{
			Container.push_back(Value);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T_Container, class T_Functor>
	void erase_if( T_Container &Container, T_Functor Functor)
	{
		T_Container::iterator it_end = Container.end();
		T_Container::iterator it = Container.begin();
		while(it != it_end)
		{
			if( Functor(*it) == false )
			{
				Container.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	template<class O, class R, class F>
	struct static_cast_ptr_adapt
	{
		static_cast_ptr_adapt(F func)
			: m_func(func)
		{
		}

		template<class I>
        inline R operator ()(I *obj)
		{
			return m_func(static_cast<O*>(obj));
		}

		F m_func;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class O, class F>
	static_cast_ptr_adapt<O,void,F> make_static_cast_ptr_adapt(F Func)
	{
		return static_cast_ptr_adapt<O,void,F>(Func);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class O, class R, class F>
		static_cast_ptr_adapt<O,R,F> make_static_cast_ptr_adapt(F Func)
	{
		return static_cast_ptr_adapt<O,R,F>(Func);
	}
}