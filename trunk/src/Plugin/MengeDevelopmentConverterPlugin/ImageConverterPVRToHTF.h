#	pragma once

#	include "DevelopmentConverter.h"

#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ImageConverterPVRToHTF
		: public DevelopmentConverter
	{
	public:
		ImageConverterPVRToHTF();
		~ImageConverterPVRToHTF();

	public:
		bool initialize() override;

    public:
		bool convert() override;

	protected:
		ArchivatorInterfacePtr m_archivator;
    };
}	// namespace Menge
