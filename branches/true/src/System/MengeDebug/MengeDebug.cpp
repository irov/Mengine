#	include "Console.h"

//////////////////////////////////////////////////////////////////////////

Menge::ConsoleInterface * console = 0;

extern "C" void dllStartPlugin( Menge::MengeInterface * _menge )
{
	Menge::LogSystemInterface * logSystem = _menge->getLogSystem();

	console = new Menge::Console( _menge );
	console->inititalize( logSystem );

	_menge->registerConsole( console );
}
//////////////////////////////////////////////////////////////////////////
extern "C" void dllShutdownPlugin( Menge::MengeInterface * _menge )
{
	delete console;
}