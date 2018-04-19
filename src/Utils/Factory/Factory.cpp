#include "Factory.h"
#include "Factorable.h"

#include "Interface/FactoryInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factory::Factory( const Char * _name )
        : m_name( _name )
        , m_count( 0 )
    {
        FACTORY_SERVICE()
            ->registerFactory( this );
    }
    //////////////////////////////////////////////////////////////////////////
    Factory::~Factory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PointerFactorable Factory::createObject()
    {
        STDEX_THREAD_GUARD_CHECK( this, "Factory::createObject" );

        ++m_count;
        intrusive_ptr_add_ref( this );

        Factorable * object = this->_createObject();
        object->setFactory( this );

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroyObject( Factorable * _object )
    {
        STDEX_THREAD_GUARD_CHECK( this, "Factory::destroyObject" );

        this->_destroyObject( _object );

        --m_count;
        intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factory::isEmptyObjects() const
    {
        return m_count == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factory::getCountObject() const
    {
        return m_count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::_destroy()
    {
        FACTORY_SERVICE()
            ->unregisterFactory( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroy()
    {
        this->_destroy();

        delete this;
    }
}
