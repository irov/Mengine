#include "Sprite.h"

#include "Interface/PrototypeServiceInterface.h"

#include "SurfaceImage.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Document.h"
#include "Kernel/Content.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Sprite::Sprite()
        : m_size( -1.f, -1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Sprite::~Sprite()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setSpriteCache( const SpriteCachePtr & _spriteCache )
    {
        m_spriteCache = _spriteCache;
    }
    //////////////////////////////////////////////////////////////////////////
    const SpriteCachePtr & Sprite::getSpriteCache() const
    {
        return m_spriteCache;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        m_resourceImage = _resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Sprite::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_fileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & Sprite::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Sprite::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setCodecType( const ConstString & _codecType )
    {
        m_codecType = _codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Sprite::getCodecType() const
    {
        return m_codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Sprite::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Sprite::_compile()
    {
        const SurfacePtr & surface = this->getSurface();

        if( surface == nullptr )
        {
            SurfacePtr surfaceImage = makeSurfaceImage_();

            MENGINE_ASSERTION_MEMORY_PANIC( surfaceImage, false );

            this->setSurface( surfaceImage );
        }

        if( ShapeQuadFixed::_compile() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Sprite::_release()
    {
        ShapeQuadFixed::_release();

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    SurfacePtr Sprite::makeSurfaceImage_()
    {
        SurfaceImagePtr surfaceImage = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( surfaceImage, nullptr );

        if( m_resourceImage != nullptr )
        {
            surfaceImage->setResourceImage( m_resourceImage );

            return surfaceImage;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_spriteCache, nullptr );

        const ResourceImagePtr & resourceImage = m_spriteCache->getResourceImage( m_fileGroup, m_filePath, m_codecType, m_size );

        this->setResourceImage( resourceImage );

        const mt::vec2f & size = resourceImage->getMaxSize();
        this->setSize( size );

        Content * content = resourceImage->getContent();
        const ConstString & codecType = content->getCodecType();
        this->setCodecType( codecType );

        surfaceImage->setResourceImage( resourceImage );

        return surfaceImage;
    }
}