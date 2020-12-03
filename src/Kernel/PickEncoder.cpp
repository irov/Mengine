#include "PickEncoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PickEncoder::PickEncoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PickEncoder::~PickEncoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::setOptions( const CodecOptions * _options )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _options );
        MENGINE_ASSERTION_TYPE( _options, const PickCodecOptions * );

        m_options = *static_cast<const PickCodecOptions *>(_options);

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickEncoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
