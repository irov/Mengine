#	pragma once

#	include "DevelopmentConverter.h"

#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ParticleConverterPTCToPTZ
		: public DevelopmentConverter
	{
	public:
		ParticleConverterPTCToPTZ();
		~ParticleConverterPTCToPTZ();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
	protected:
		bool convert_( const FilePath & _input, const FilePath & _output );

	protected:
		ArchivatorInterfacePtr m_archivator;
    };
}	// namespace Menge
