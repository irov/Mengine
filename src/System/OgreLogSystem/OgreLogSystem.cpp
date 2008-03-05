#	include "OgreLogSystem.h"

#	include "Ogre.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( LogSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}
	
	*_interface = new OgreLogSystem();
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( LogSystemInterface* _interface )
{
	if( _interface )
	{
		delete _interface;
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreLogSystem::logMessage( const char * _message, bool _maskDebug )
{
	Ogre::LogManager::getSingleton()
		.logMessage( _message, Ogre::LML_NORMAL, _maskDebug );
}