#pragma once

#include "Factory/Factory.h"
#include "Factory/FactorableUnique.h"

#include <typeinfo>

namespace Mengine
{
	template<class Type>
	class FactoryDefault
		: public Factory
	{
	public:
		FactoryDefault()
            : Factory( typeid(Type).name() )
		{
		}

    protected:
		Factorable * _createObject() override
		{
            Type * t = Helper::allocateT<Type>();

			return t;
		}

        void _destroyObject( Factorable * _obj ) override
		{
			Helper::freeT( _obj );
		}
	};
}
