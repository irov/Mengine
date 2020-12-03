#include "ImageEncoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageEncoder::ImageEncoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoder::~ImageEncoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::setOptions( const CodecOptions * _options )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _options );
        MENGINE_ASSERTION_TYPE( _options, const ImageCodecOptions * );

        m_options = *static_cast<const ImageCodecOptions *>(_options);
        
        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
