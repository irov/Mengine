#	include <windows.h>

#	include "Menge/Application.h"

#	include <direct.h>

#	include "Menge/Utils.h"

#	include "Menge/holder.h"

#	include "Compiler.h"

#	include "ResourceCallback.h"

#	include "LoggerConsole.h"

#	include "Interface/LogSystemInterface.h"

//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{ 
	Menge::String _commandLine = lpCmdLine;

	if( _commandLine.empty() )
	{
		return 0;
	}

	TStringVector files = Menge::Utils::split(_commandLine);

	if( files.size() > 2 )
	{
		return 0;
	}

	Menge::String config_file = files[0];
	Menge::String output = files[1];

	Compiler simpleCompiler;

	simpleCompiler.registerPyCallback( "copyFolder", "copyFolder" );
	simpleCompiler.registerPyCallback( "copyFile", "copyFile" );
	simpleCompiler.registerCppCallback( "convertResources", new ResourceCallback() );
	
	Menge::String workdir = config_file.substr(0,config_file.find_last_of("/\\"));

	_chdir( workdir.c_str() );

	Menge::Application * m_menge = new Menge::Application(0,"",false);
	Menge::LogSystemInterface * m_logSystem = m_menge->initializeLogSystem();
	Menge::LoggerConsole * m_loggerConsole = new Menge::LoggerConsole();
	m_logSystem->registerLogger( m_loggerConsole );

	if( m_menge->initialize( config_file, "", false ) == false )
	{
		return false;
	}

	Menge::Game * game = Menge::Holder<Menge::Game>::hostage();

	simpleCompiler.setOutputFolder( output );
	simpleCompiler.compile( game );

	return 0;
}
//////////////////////////////////////////////////////////////////////////