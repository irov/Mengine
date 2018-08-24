#include "Kernel/Updatable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Updatable::Updatable()
        : m_updateRevision( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Updatable::~Updatable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	void Updatable::update( const UpdateContext * _context )
	{
        m_updateRevision = _context->revision;

        this->_update( _context );
	}
    //////////////////////////////////////////////////////////////////////////
    void Updatable::setUpdateRevision( uint32_t _revision )
    {
        m_updateRevision = _revision;
    }
}