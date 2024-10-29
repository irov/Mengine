#include "VideoDecoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VideoDecoder::VideoDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VideoDecoder::~VideoDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo, "invalid set codec data info" );
        MENGINE_ASSERTION_TYPE( _dataInfo, const VideoCodecDataInfo *, "invalid set codec data info" );

        m_dataInfo = *static_cast<const VideoCodecDataInfo *>(_dataInfo);
    }
    //////////////////////////////////////////////////////////////////////////
    const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const
    {
        return &m_dataInfo;
    }
    //////////////////////////////////////////////////////////////////////////
}
