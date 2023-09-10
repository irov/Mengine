#include "ResourceImageSequenceValidator.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "ResourceValidateServiceInterface.h"

#include "Kernel/TextureHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"

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
    bool ResourceImageSequenceValidator::_validate( const ResourceImageSequencePtr & _resource ) const
    {
        bool successful = true;

        size_t total_memory = 0;

        const VectorFrameImageSequence & sequences = _resource->getSequence();

        for( const FrameImageSequence & sequence : sequences )
        {
            const ResourceImagePtr & resourceImage = sequence.resourceImage;

            if( RESOURCEVALIDATE_SERVICE()
                ->validateResource( resourceImage ) == false )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid validate sequence resource '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , resourceImage->getName().c_str()
                );

                successful = false;
            }

            const mt::vec2f & size = resourceImage->getSize();

            uint32_t width = Helper::getTexturePow2( (uint32_t)size.x );
            uint32_t height = Helper::getTexturePow2( (uint32_t)size.y );

            uint32_t channels = 3;

            if( resourceImage->hasAlpha() == true )
            {
                channels = 4;
            }

            EPixelFormat pixelFormat = Helper::findBestPixelFormat( channels, 1 );

            size_t textureSize = Helper::getTextureMemorySize( width, height, pixelFormat );

            total_memory += textureSize;
        }

        uint32_t Limit_AnimationMemoryLimit = CONFIG_VALUE( "Limit", "AnimationMemoryLimit", 4194304U ); //4mb
        float Limit_AnimationMemoryLimitThresholdCoeff = CONFIG_VALUE( "Limit", "AnimationMemoryLimitThresholdCoeff", 4.f );

        if( total_memory > Limit_AnimationMemoryLimit && Limit_AnimationMemoryLimit != 0U )
        {
            if( float( total_memory ) / float( Limit_AnimationMemoryLimit ) > Limit_AnimationMemoryLimitThresholdCoeff )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' overflow %.2fmb max video memory %.2fmb (coeff %.2f)"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , float( total_memory ) / (1024.f * 1024.f)
                    , float( Limit_AnimationMemoryLimit ) / (1024.f * 1024.f)
                    , float( total_memory ) / float( Limit_AnimationMemoryLimit )
                );

                successful = false;
            }
            else
            {
                LOGGER_MESSAGE_RELEASE_WARNING( "resource '%s' group '%s' overflow %.2fmb max video memory %.2fmb (coeff %.2f)"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , float( total_memory ) / (1024.f * 1024.f)
                    , float( Limit_AnimationMemoryLimit ) / (1024.f * 1024.f)
                    , float( total_memory ) / float( Limit_AnimationMemoryLimit )
                );
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}