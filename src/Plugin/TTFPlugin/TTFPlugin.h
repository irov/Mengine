#	pragma once

#	include "Core/PluginBase.h"

#	include "TTFServiceInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class TTFPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "TTF" )

	public:
		TTFPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

    protected:
        bool createTTFFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName );
	};
}