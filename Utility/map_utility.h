#	pragma once

namespace Utility
{

	template<class T_Iterator, class T_Function> inline
		void map_for_each_second(T_Iterator First, T_Iterator Last, T_Function Func)
	{
		for (; First != Last; ++First)
			Func(First->second);
	}

	template<class T_Iterator, class T_Function> inline
		void map_for_each_first(T_Iterator First, T_Iterator Last, T_Function Func)
	{
		for (; First != Last; ++First)
			Func(First->first);
	}

	template<class T_Contanier, class T_Key> inline
		void map_erase_ptr(T_Contanier Container, T_Key Key)
	{
		typename T_Contanier::iterator it_find = Container.find(Key);

		if( it_find != Container.end() )
		{
			delete it_find->second;
			Container.erase(it_find);
		}
	}

	template<class T_Contanier>
	class FMapSecondBackInserter
	{
	public:
		FMapSecondBackInserter( T_Contanier &Container )
		: m_Container(Container)
		{
		}

		FMapSecondBackInserter( const FMapSecondBackInserter &F )
			: m_Container(F.m_Container)
		{
		}

		template<class T_Value>
			void operator() ( const T_Value &Value )
		{
			
		}

		template<class T_Value>
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
		T_Contanier &m_Container;
	};

	template<class T_Contanier>
	inline FMapSecondBackInserter<T_Contanier> 
	map_second_back_inserter( T_Contanier &Container)
	{
		return FMapSecondBackInserter<T_Contanier>(Container);
	}

	template<typename T_Class, typename T_Member>
	class FCompareMember 
	: public std::unary_function<T_Class, bool>
	{
		typedef T_Member T_Class::* TMemberPtr;
	public:
		FCompareMember(TMemberPtr MemPtr, const T_Member& MemVal) 
			: m_MemPtr(MemPtr)
			, m_MemVal(MemVal) 
		{
		}

		FCompareMember( const FCompareMember &F) 
			: m_MemPtr(F.m_MemPtr)
			, m_MemVal(F.m_MemVal) 
		{
		}

		template<typename ChainedPtr>
			bool operator()(const ChainedPtr& x)const
		{
			return operator()(*x);
		}

		bool operator()(const T_Class& obj) const
		{
			return obj.*m_MemPtr == m_MemVal;
		}

	private:
		TMemberPtr			m_MemPtr;
		const T_Member	&	m_MemVal;
	};

	template<typename T_Class, typename T_Member>
		inline FCompareMember<T_Class,T_Member> 
		compare_mem( T_Member T_Class::* MemPtr, const T_Member& MemVal)
	{
		return FCompareMember<T_Class,T_Member>(MemPtr,MemVal);
	}

	class delete_ptr
	{
	public:
		template<class T>
		void operator()( T *Object)const
		{
			delete Object;
		}

		template<class T1, class T2>
			void operator()(const std::pair<const T1,T2> &Pair)const
		{
			delete Pair.second;
		}
	};

	template<typename T_Contanier, typename T_Function>
	void erase_container( T_Contanier & Container, T_Function &Function )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			Function);

		Container.clear();
	}

	template<typename T_Contanier>
	void erase_ptr_container( T_Contanier & Container)
	{
		std::for_each(
			Container.begin(),
			Container.end(),
            delete_ptr());

		Container.clear();
	}

	template<typename T_Contanier, typename T_Function>
	void for_each( T_Contanier &Container, T_Function &Function)
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			Function);
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class>
		void for_each_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)() )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class>
	void for_each_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)() const )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class>
		void map_for_each_second_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)() )
	{
		map_for_each_second(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class>
		void map_for_each_second_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)() const )
	{
		map_for_each_second(
			Container.begin(),
			Container.end(),
			std::mem_fun(Function));
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void for_each_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)(T_Arg) const,const T_Arg &Value )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::bind2nd(std::mem_fun(Function),Value));
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void for_each_mem_fun( T_Contanier &Container, T_Result (T_Class::*Function)(T_Arg),const T_Arg &Value )
	{
		std::for_each(
			Container.begin(),
			Container.end(),
			std::bind2nd(std::mem_fun(Function),Value));
	}


	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void map_find_and_execute( T_Contanier &Container,const typename T_Contanier::key_type & FindValue, T_Result (T_Class::*Function)(T_Arg)const,const T_Arg &Value )
	{
		T_Contanier::const_iterator It_find = Container.find(FindValue);
		if( It_find != Container.end() )
		{
			std::bind2nd(std::mem_fun(Function),Value)(It_find->second);
		}
	}

	template<
		typename T_Contanier, 
		typename T_Result,
		typename T_Class,
		typename T_Arg> inline 
		void map_find_and_execute( T_Contanier &Container,const typename T_Contanier::key_type & FindValue, T_Result (T_Class::*Function)(T_Arg),const T_Arg &Value )
	{
		T_Contanier::const_iterator It_find = Container.find(FindValue);
		if( It_find != Container.end() )
		{
			std::bind2nd(std::mem_fun(Function),Value)(It_find->second);
		}
	}


	template<
		typename T_Contanier, 
		typename T_Function> inline 
		void map_find_and_execute( T_Contanier &Container,const typename T_Contanier::key_type & FindValue, T_Function &Function )
	{
		T_Contanier::const_iterator It_find = Container.find(FindValue);
		if( It_find != Container.end() )
		{
			Function(It_find->second);
		}
	}

	template<
		typename T_Contanier>
		void copy_container( const T_Contanier &In, T_Contanier &To)
	{
		std::copy(
			In.begin(),
			In.end(),
			std::back_inserter(To)
			);
	}

	template< 
		typename T_Contanier,
		typename T_Function>
		void erase_container_if( T_Contanier &Container, T_Function &Function)
	{
		Container.erase(
			std::remove_if(
			Container.begin(),
			Container.end(),
			Function),
			Container.end());
	}

	template<class T_Map>
		typename T_Map::iterator map_insert( T_Map &Map, const typename T_Map::key_type &Key, const typename T_Map::mapped_type &Value)
	{
		return Map.insert(std::make_pair(Key,Value)).first;
	}

	template<class T_Map>
		typename T_Map::mapped_type map_ptr_find( T_Map &Map, const typename T_Map::key_type &Key )
	{
		TMapScriptFunction::iterator it_find = m_MapScriptFunction.find(Name);
		if( it_find != m_MapScriptFunction.end())
		{
			return	it_find->second;
		}
		return 0;
	}

	template<class T_Contanier>
		void vector_smart_erase(T_Contanier &Container,const typename T_Contanier::iterator &It)
	{
		const typename T_Contanier::value_type &Value = Container.back();
		(*It) = Value;
		Container.pop_back();
	}

		template<class T_Contanier>
		void vector_smart_erase(T_Contanier &Container, const typename T_Contanier::value_type &Value)
	{
		typename T_Contanier::iterator It = std::find(Container.begin(),Container.end(),Value);
		if( It != Container.end() )
		{
			vector_smart_erase(Container,It);
		}
	}

}