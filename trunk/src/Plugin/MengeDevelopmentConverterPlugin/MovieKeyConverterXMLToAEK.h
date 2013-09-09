#	pragma once

#   include "DevelopmentConverter.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include "Menge/MovieFramePack.h"


namespace Menge
{
	class MovieKeyConverterXMLToAEK
        : public DevelopmentConverter
	{
	public:
		MovieKeyConverterXMLToAEK( ServiceProviderInterface * _serviceProvider );
		~MovieKeyConverterXMLToAEK();

	public:
		bool initialize() override;

	public:
		bool convert() override;

	protected:
		bool loadFramePak_( MovieFramePack & _movieFramePack );
		bool writeFramePak_( MovieFramePack & _movieFramePack );	
	};
}