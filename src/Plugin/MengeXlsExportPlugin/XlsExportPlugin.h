#pragma once

#include "Interface/NotificationServiceInterface.h"

#include "Core/PluginBase.h"
#include "Core/ConstString.h"

#include "XlsScriptLogger.h"

namespace Mengine
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
		void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );

	protected:
		bool proccess_();

	protected:
		void warning_(const wchar_t * _msg);
		void error_( const wchar_t * _msg );

	protected:
		XlsScriptLogger * m_warninglogger;
		XlsScriptLogger * m_errorLogger;

		ObserverInterfacePtr m_observerChangeLocale;
	};
}