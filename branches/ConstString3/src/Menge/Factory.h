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

	public:
		virtual void destroyObject( Factorable * _node ) = 0;

	protected:
		virtual Factorable * _createObject( const ConstString & _type ) = 0;
	};
}
