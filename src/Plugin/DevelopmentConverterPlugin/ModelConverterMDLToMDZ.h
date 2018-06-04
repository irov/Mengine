#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchiveInterface.h"

namespace Mengine
{
	class ModelConverterMDLToMDZ
		: public DevelopmentConverter
	{
	public:
		ModelConverterMDLToMDZ();
		~ModelConverterMDLToMDZ() override;

	public:
		bool _initialize() override;

    public:
		bool convert() override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );

	protected:
		ArchivatorInterfacePtr m_archivator;
    };
}	
