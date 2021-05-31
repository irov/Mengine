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
    PickDecoder::~PickDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PickDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const PickCodecDataInfo * );

        m_dataInfo = *static_cast<const PickCodecDataInfo *>(_dataInfo);
    }
    //////////////////////////////////////////////////////////////////////////
    const PickCodecDataInfo * PickDecoder::getCodecDataInfo() const
    {
        return &m_dataInfo;
    }
    //////////////////////////////////////////////////////////////////////////
}
