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
			T * t = new T();

			return t;
		}

        void _destroyObject( Factorable * _obj ) override
		{
			delete static_cast<T *>(_obj);
		}

	public:
		T * createObjectT()
		{
			Factorable * obj = this->createObject();

			T * t = static_cast<T *>(obj);

			return t;
		}

	};
}
