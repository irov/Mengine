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
		Factorable * createObject( const ConstString & _type );
		void destroyObject( Factorable * _node );

	public:
		std::size_t countObject() const;

	protected:
		virtual Factorable * _createObject( const ConstString & _type ) = 0;
		virtual void _destroyObject( Factorable * _node ) = 0;

	protected:
		std::size_t m_count;
	};
}
