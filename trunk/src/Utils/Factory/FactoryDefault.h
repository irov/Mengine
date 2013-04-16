	#	pragma once

#	include "Factory.h"

namespace Menge
{
	template<class T>
	class FactoryDefault
		: public Factory
	{
    public:
        T * createObjectT()
        {
            Factorable * obj = this->createObject();

            return static_cast<T *>(obj);
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
