#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DecoderSystemInterface;

	class XlsExportPlugin
		: public PluginInterface
	{
	public:
		XlsExportPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		bool proccess( const String & _projectName );
	};
}