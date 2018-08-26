#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class HotspotImageConverterPNGToHIT
        : public DevelopmentConverter
    {
    public:
        HotspotImageConverterPNGToHIT();
        ~HotspotImageConverterPNGToHIT() override;

    public:
        bool _initialize() override;

    public:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    public:
        bool convert() override;

    protected:
        uint32_t calcMimMapBufferLevelAndSize_( uint32_t _width, uint32_t _height, uint32_t & _level );
        void makeMipMapLevel_( uint8_t * _buffer, uint32_t _width, uint32_t _height, uint32_t _level );
    };
}
