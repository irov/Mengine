#	pragma once

#   include "DevelopmentConverter.h"

#   include "Interface/ArchiveInterface.h"

#	include "Config/Blobject.h"

namespace Menge
{
	class MovieKeyConverterXMLToAEK
        : public DevelopmentConverter
	{
	public:
		MovieKeyConverterXMLToAEK();
		~MovieKeyConverterXMLToAEK();

	public:
		bool initialize() override;

	public:
		bool convert() override;

	public:
		bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

	protected:
		bool loadFramePak_( TBlobject & _buffer );
		bool writeFramePak_( const TBlobject & _buffer );

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}