#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class HotspotImageConverterPNGToHIT
		: public DevelopmentConverter
	{
	public:
		HotspotImageConverterPNGToHIT( ServiceProviderInterface * _serviceProvider );
		~HotspotImageConverterPNGToHIT();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
        size_t calcMimMapBufferLevelAndSize_( size_t _width, size_t _height, size_t & _level );
        void makeMipMapLevel_( unsigned char * _buffer, size_t _width, size_t _height, size_t _level );
    };
}	// namespace Menge
