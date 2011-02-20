	#	pragma once

#	include "Factory.h"

namespace Menge
{
	template<class T>
	class FactoryDefault
		: public Factory
	{
	public:
		Factorable * _createObject( const ConstString & _type ) override
		{
			T * t = new T();
			return t;
		}

	public:
		void _destroyObject( Factorable * _node ) override
		{
			delete _node;
		}
	};

	namespace Helper
	{
		template<class T>
		Factory * createFactoryDefault()
		{
			return new FactoryDefault<T>();
		}
	}
}
