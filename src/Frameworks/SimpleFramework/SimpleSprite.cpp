#include "SimpleSprite.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/SurfaceImage.h"

namespace Mengine
{
    namespace Helper
    {
        ShapeQuadSizePtr createSimpleSprite( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const mt::vec2f & _textureSize, const mt::vec2f & _size, const DocumentPtr & _doc )
        {
            ResourceImageDefaultPtr resource = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), _doc );

            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), _doc );
            
            content->setFileGroup( _fileGroup );
            content->setFilePath( _filePath );

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

            SurfaceImagePtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), _doc );

            surface->setResourceImage( resource );

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            ShapeQuadSizePtr shape = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadSize" ), _doc );

            shape->setSurface( surface );
            shape->setSize( _size );

            return shape;
        }
    }
}