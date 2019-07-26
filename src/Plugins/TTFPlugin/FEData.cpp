#include "FEData.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEData::FEData()
        : m_bundle( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEData::~FEData()
    {
        if( m_bundle != nullptr )
        {
            fe_bundle_free( m_bundle );
            m_bundle = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEData::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEData::release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer FEData::allocateMemory( size_t _size ) const
    {
        MENGINE_UNUSED( _size );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEData::setFEBundle( fe_bundle * _bundle )
    {
        m_bundle = _bundle;
    }
    //////////////////////////////////////////////////////////////////////////
    fe_bundle * FEData::getFEBundle() const
    {
        return m_bundle;
    }
}