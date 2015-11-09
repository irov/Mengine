#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/PluginBase.h"

namespace Menge
{
	class XlsExportPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "XlsExport" )

	public:
		XlsExportPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		bool proccess_( const char * _projectName );

	protected:
		void error_( const wchar_t * _msg );
	};
}