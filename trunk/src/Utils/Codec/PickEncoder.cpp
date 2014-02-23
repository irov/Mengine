#	include "PickEncoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickEncoder::PickEncoder()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::setOptions( CodecOptions * _options )
    {
        if( _options != NULL )
        {
            m_options = *static_cast<PickCodecOptions *>(_options);
        }

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::_invalidateOptions()
    {
        return true;
    }
}
