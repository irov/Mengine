#	include "ResourceImage.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage()
        : m_texture(nullptr)
        , m_textureAlpha(nullptr)
        , m_maxSize(0.f, 0.f)
        , m_size(0.f, 0.f)
		, m_offset(0.f, 0.f)
        , m_isAlpha(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::~ResourceImage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
    void ResourceImage::_release()
    {
        if( m_texture != nullptr )
        {
            const FilePath & filePath = m_texture->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImage::_release %s release texture %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            m_texture = nullptr;
        }

        if( m_textureAlpha != nullptr )
        {
            const FilePath & filePath = m_textureAlpha->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImage::_release %s release texture alpha %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            m_textureAlpha = nullptr;
        }
    }
	/////////////////////////////////////////////////////////////////////////
	size_t ResourceImage::memoryUse() const
	{
		size_t memory = ResourceReference::memoryUse();

		if( m_texture != nullptr )
		{
			memory += m_texture->getMemoryUse();
		}

		if( m_textureAlpha != nullptr )
		{
			memory += m_textureAlpha->getMemoryUse();
		}

		return memory;
	}
}
