#pragma once

#include "Kernel/Observable.h"

#include "Kernel/PluginBase.h"
#include "Kernel/ConstString.h"

#include "XlsScriptLogger.h"

namespace Mengine
{
	class XlsExportPlugin
		: public PluginBase
        , public Observable
	{
		PLUGIN_DECLARE( "XlsExport" )

	public:
		XlsExportPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );

	protected:
		bool proccess_();

	protected:
		void warning_(const wchar_t * _msg);
		void error_( const wchar_t * _msg );

	protected:
		XlsScriptLogger * m_warninglogger;
		XlsScriptLogger * m_errorLogger;
	};
}