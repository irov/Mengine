#include "FactoryBase.h"
#include "Factorable.h"

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryBase::FactoryBase()
#if defined(MENGINE_DEBUG)
        : m_register( false )
#endif
    {
#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->registerFactory( this );

            m_register = true;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryBase::~FactoryBase()
    {
#if defined(MENGINE_DEBUG)
        if( m_register == true )
        {
            FACTORY_SERVICE()
                ->unregisterFactory( this );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryBase::setType( const ConstString & _type )
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FactoryBase::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryBase::isEmptyObjects() const
    {
        uint32_t counter = m_count.getReferenceCount();

        if( counter != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryBase::getCountObject() const
    {
        uint32_t count = m_count.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    void FactoryBase::foreachFactorables( const LambdaFactorable & _lambda )
    {
        for( Factorable * factorable : m_factorables )
        {
            if( _lambda( factorable ) == false )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
