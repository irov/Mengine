#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UnknownInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/FilePath.h"

#include "Config/String.h"

#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownTTFFontInterface
        : public UnknownInterface
    {
    public:
        virtual void setSystem( bool _system ) = 0;
        virtual bool getSystem() const = 0;

        virtual void setTTFPath( const FilePath & _ttfPath ) = 0;
        virtual const FilePath & getTTFPath() const = 0;

        virtual void setTTFFileGroup( const FileGroupInterfacePtr & _fileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getTTFFileGroup() const = 0;

        virtual void setTTFFEPath( const FilePath & _ttfPath ) = 0;
        virtual const FilePath & getTTFFEPath() const = 0;

        virtual void setTTFFEName( const ConstString & _ttfPath ) = 0;
        virtual const ConstString & getTTFFEName() const = 0;

        virtual void setTTFFEFileGroup( const FileGroupInterfacePtr & _fileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getTTFFEFileGroup() const = 0;

        virtual void setFESample( uint32_t _FESample ) = 0;
        virtual uint32_t getFESample() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownTTFFontInterface> UnknownTTFFontInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TTFTextureGlyphProviderInterface
        : public Mixin
    {
    public:
        virtual bool onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class TTFAtlasServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TTFAtlasService" )

    public:
        virtual RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _border, uint32_t _channel, TTFTextureGlyphProviderInterface * _provider, mt::uv4f & _uv, const DocumentPtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TTFATLAS_SERVICE()\
    ((Mengine::TTFAtlasServiceInterface *)SERVICE_GET(Mengine::TTFAtlasServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define TTFFONT_SERVICE()\
    ((Mengine::TTFFontManagerInterface *)SERVICE_GET(Mengine::TTFFontManagerInterface))
//////////////////////////////////////////////////////////////////////////