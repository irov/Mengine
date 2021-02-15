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
        ShapeQuadFixedPtr createSimpleSprite( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const mt::vec2f & _maxSize, const DocumentPtr & _doc )
        {
            ResourceImageDefaultPtr resource = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), _doc );

            ContentInterface * content = resource->getContent();

            content->setFileGroup( _fileGroup );
            content->setFilePath( _filePath );

            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( _filePath );

            content->setCodecType( codecType );

            resource->setMaxSize( _maxSize );
            resource->setSize( _maxSize );

            if( resource->initialize() == false )
            {
                return nullptr;
            }

            SurfaceImagePtr surface = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FUNCTION );

            surface->setResourceImage( resource );

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            ShapeQuadFixedPtr shape = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FUNCTION );

            shape->setSurface( surface );

            return shape;
        }
    }
}