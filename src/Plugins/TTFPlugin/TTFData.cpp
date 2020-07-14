#include "TTFData.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFData::TTFData()
        : m_face( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFData::~TTFData()
    {
        if( m_face != nullptr )
        {
            FT_Done_Face( m_face );
            m_face = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFData::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFData::release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFData::setTTFMemory( const MemoryInterfacePtr & _memory )
    {
        m_memory = _memory;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & TTFData::getTTFMemory() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFData::setFTFace( FT_Face _face )
    {
        m_face = _face;
    }
    //////////////////////////////////////////////////////////////////////////
    FT_Face TTFData::getFTFace() const
    {
        return m_face;
    }
}