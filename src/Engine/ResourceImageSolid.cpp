#include "ResourceImageSolid.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

#include "Interface/RenderSystemInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSolid::ResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSolid::~ResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSolid::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid *>(_meta);

        m_textureColor = metadata->get_Color_Value();

        const mt::vec2f & size = metadata->get_Size_Value();

        m_maxSize = size;
        m_size = size;
        m_offset = mt::vec2f( 0.f, 0.f );

        m_hasAlpha = false;

        m_texture = nullptr;
        m_textureAlpha = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSolid::_compile()
    {
        this->setPow2( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}