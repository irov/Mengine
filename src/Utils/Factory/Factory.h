#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class Factorable;

	class Factory
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
}
