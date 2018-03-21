#include "ImageEncoder.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ImageEncoder::ImageEncoder()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::setOptions( CodecOptions * _options )
    {
        if( _options != nullptr )
        {
            m_options = *static_cast<ImageCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoder::_invalidateOptions()
    {
        return true;
    }
}
