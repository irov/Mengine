#	include "Factory.h"
#	include "Factorable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factorable * Factory::createObject()
	{
		Factorable * object = this->_createObject();
		object->setFactory( this );
		return object;
	}
}
