#	include "Factory.h"
#	include "Factorable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Factory::Factory()
		: m_count(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factory::~Factory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * Factory::createObject()
	{
		Factorable * object = this->_createObject();
		object->setFactory( this );

		++m_count;

		intrusive_ptr_add_ref( this );

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::destroyObject( Factorable * _object )
	{
		--m_count;

		this->_destroyObject( _object );

		intrusive_ptr_dec_ref( this );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Factory::countObject() const
	{
		return m_count;
	}
}
