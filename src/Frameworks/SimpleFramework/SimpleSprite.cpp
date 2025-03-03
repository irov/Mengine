#include "SimpleSprite.h"

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        ShapeQuadSizePtr createSimpleSprite( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const mt::vec2f & _textureSize, const mt::vec2f & _size, const DocumentInterfacePtr & _doc )
        {
            ResourceImageDefaultPtr resource = Helper::generateResourceFactorable<ResourceImageDefault>( _doc );

            ContentInterfacePtr content = Helper::makeFileContent( _fileGroup, _filePath, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid make content '%s'"
                , _filePath.c_str()
            );

            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( _filePath );

            content->setCodecType( codecType );

            resource->setContent( content );

            resource->setMaxSize( _textureSize );
            resource->setSize( _textureSize );

            if( resource->initialize() == false )
            {
                return nullptr;
            }

            SurfaceImagePtr surface = Helper::generateSurfaceFactorable<SurfaceImage>( _doc );

            surface->setResourceImage( resource );

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            ShapeQuadSizePtr shape = Helper::generateNodeFactorable<ShapeQuadSize>( _doc );

            shape->setSurface( surface );
            shape->setSize( _size );

            return shape;
        }
    }
}