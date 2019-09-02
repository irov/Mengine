#include "ResourceImageSequenceValidator.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "ResourceValidateServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSequenceValidator::ResourceImageSequenceValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSequenceValidator::~ResourceImageSequenceValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSequenceValidator::_validate( const ResourceImageSequencePtr & _resource )
    {
        size_t total_memory = 0;

        const VectorFrameImageSequence & sequences = _resource->getSequence();

        for( const FrameImageSequence & sequence : sequences )
        {
            //ResourceImagePtr resourceImage;
            //if( RESOURCE_SERVICE()
            //    ->hasResourceT<ResourceImagePtr>( sequence.resourceName, &resourceImage ) == false )
            //{
            //    LOGGER_ERROR( "resource '%s' group '%s' not found image resource '%s'"
            //        , _resource->getName().c_str()
            //        , _resource->getGroupName().c_str()
            //        , sequence.resourceName.c_str()
            //    );

            //    return false;
            //}
            const ResourceImagePtr & resourceImage = sequence.resource;

            if( RESOURCEVALIDATE_SERVICE()
                ->validResource( resourceImage ) == false )
            {
                LOGGER_ERROR( "resource '%s' group '%s' invalid validate sequence resource '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , resourceImage->getName().c_str()
                );

                return false;
            }

            const mt::vec2f & size = resourceImage->getSize();

            uint32_t width = Helper::getTexturePOW2( (uint32_t)size.x );
            uint32_t height = Helper::getTexturePOW2( (uint32_t)size.y );

            uint32_t channels = 3;

            if( resourceImage->hasAlpha() == true )
            {
                channels = 4;
            }

            size_t textureSize = Helper::getTextureMemorySize( width, height, channels, 1, PF_UNKNOWN );

            total_memory += textureSize;
        }

        uint32_t animationMemoryLimit = CONFIG_VALUE( "Limit", "AnimationMemoryLimit", 4194304U ); //4mb

        if( total_memory > animationMemoryLimit && animationMemoryLimit != 0U )
        {
            LOGGER_ERROR( "resource '%s' group '%s' overflow %.2fmb max video memory %.2fmb (coeff %f)"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , float( total_memory ) / (1024.f * 1024.f)
                , float( animationMemoryLimit ) / (1024.f * 1024.f)
                , float( total_memory ) / float( animationMemoryLimit )
            );

            return false;
        }

        return true;
    }
}