#	pragma once

#   include "Factory/FactorablePtr.h"

#   include <stddef.h>

namespace Menge
{
	class Factorable;

	class Factory
		: public FactorablePtr
	{
	public:
		Factory();
		virtual ~Factory();

	public:
		Factorable * createObject();
		void destroyObject( Factorable * _object );

	public:
		size_t countObject() const;

	protected:
		virtual Factorable * _createObject() = 0;
		virtual void _destroyObject( Factorable * _object ) = 0;

	protected:
		size_t m_count;
	};

	typedef stdex::intrusive_ptr<Factory> FactoryPtr;
}
