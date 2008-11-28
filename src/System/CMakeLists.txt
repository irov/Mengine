

ADD_SUBDIRECTORY( AstralaxParticleSystem )
ADD_SUBDIRECTORY( Box2DPhysicSystem )
ADD_SUBDIRECTORY( OISInputSystem )
ADD_SUBDIRECTORY( MengeLogSystem )
ADD_SUBDIRECTORY( SoundSystem )

if( WIN32 )
	ADD_SUBDIRECTORY( MengeWin32FileSystem )
elseif( APPLE )
	ADD_SUBDIRECTORY( MengeMacOSFileSystem )
endif( WIN32 )

if( WIN32 )
	ADD_SUBDIRECTORY( HGERenderSystem )
else( WIN32 )
	ADD_SUBDIRECTORY( OGLRenderSystem )
endif( WIN32 )

