#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Factorable;

	class Factory
	{
	public:		
		Factorable * createObject();

	public:
		virtual void destroyObject( Factorable * _node ) = 0;

	protected:
		virtual Factorable * _createObject() = 0;
	};
}