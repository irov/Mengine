#	pragma once

#	include "Factory.h"
#	include "FactorableUnique.h"

namespace Menge
{
	template<class T>
	class FactoryDefault
		: public Factory
	{
	public:
		FactoryDefault()
		{
		}

    protected:
		Factorable * _createObject() override
		{
			T * t = Helper::allocateT<T>();

			return t;
		}

        void _destroyObject( Factorable * _obj ) override
		{
			Helper::freeT( _obj );
		}
	};
}
