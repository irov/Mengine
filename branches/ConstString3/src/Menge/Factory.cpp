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
	Factorable * Factory::createObject( const ConstString & _type )
	{
		Factorable * object = this->_createObject( _type );
		object->setFactory( this );

		++m_count;

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::destroyObject( Factorable * _node )
	{
		if( m_count == 0 )
		{
			printf("Factory error");
		}

		--m_count;

		this->_destroyObject( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Factory::countObject() const
	{
		return m_count;
	}
}
