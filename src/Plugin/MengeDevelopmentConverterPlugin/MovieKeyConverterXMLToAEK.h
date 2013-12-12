#	pragma once

#   include "DevelopmentConverter.h"

#	include "Config/Typedef.h"
#	include "Config/Blobject.h"
#	include "Core/ConstString.h"

#	include "Menge/MovieFramePack.h"


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
	};
}