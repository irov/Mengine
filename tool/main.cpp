#	include <windows.h>

#	include "Menge/Application.h"

#	include <direct.h>

#	include "Menge/Utils.h"

#	include "Menge/holder.h"

#	include "Compiler.h"

#	include "Interface/LogSystemInterface.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{ 
	if( argc != 3 )
	{
		return 0;
	}

	Compiler simpleCompiler;

	Menge::String config_file = argv[1];
	Menge::String output = argv[2];

	Menge::String workdir = config_file.substr(0,config_file.find_last_of("/\\"));

	_chdir( workdir.c_str() );

	Menge::Application * m_menge = new Menge::Application(0,"",false);
	Menge::LogSystemInterface * m_logSystem = m_menge->initializeLogSystem();

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