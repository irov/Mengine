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
    bool SoundDecoder::setOptions( const CodecOptions * _options )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _options );
        MENGINE_ASSERTION_TYPE( _options, const SoundCodecOptions * );

        m_options = *static_cast<const SoundCodecOptions *>(_options);
        
        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoder::_invalidateOptions()
    {
        return true;
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
