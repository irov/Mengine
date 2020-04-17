#include "PickDecoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PickDecoder::PickDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::setOptions( const CodecOptions * _options )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _options, false );
        MENGINE_ASSERTION_TYPE( _options, const PickCodecOptions * );

        m_options = *static_cast<const PickCodecOptions *>(_options);
        
        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickDecoder::_invalidateOptions()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC_VOID( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const PickCodecDataInfo * );

        m_dataInfo = *static_cast<const PickCodecDataInfo *>(_dataInfo);
    }
    //////////////////////////////////////////////////////////////////////////
    const PickCodecDataInfo * PickDecoder::getCodecDataInfo() const
    {
        return &m_dataInfo;
    }
}
