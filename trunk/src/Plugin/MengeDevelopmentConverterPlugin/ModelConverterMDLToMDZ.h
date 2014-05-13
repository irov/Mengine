#	pragma once

#	include "DevelopmentConverter.h"

#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ModelConverterMDLToMDZ
		: public DevelopmentConverter
	{
	public:
		ModelConverterMDLToMDZ();
		~ModelConverterMDLToMDZ();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );

	protected:
		ArchivatorInterfacePtr m_archivator;
    };
}	// namespace Menge
