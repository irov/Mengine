#include "SoundDecoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundDecoder::SoundDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoder::~SoundDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const SoundCodecDataInfo * );

        m_dataInfo = *static_cast<const SoundCodecDataInfo *>(_dataInfo);
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundCodecDataInfo * SoundDecoder::getCodecDataInfo() const
    {
        return &m_dataInfo;
    }
    //////////////////////////////////////////////////////////////////////////
}
