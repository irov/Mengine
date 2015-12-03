#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	class Consts
        : public ServiceBase<ServiceInterface>
	{
        SERVICE_DECLARE("Consts")

	public:
		Consts();

    public:
		bool _initialize() override;
		void _finalize() override;

#	define DECL_CONST( Const ) public: ConstString c_##Const

		DECL_CONST( dir );
		DECL_CONST( pak );
		DECL_CONST( zip );
		DECL_CONST( memory );
        DECL_CONST( eng );
		
        DECL_CONST( Main );
		DECL_CONST( Homeless );
		DECL_CONST( Node );
		DECL_CONST( Game );
		DECL_CONST( Default );
		DECL_CONST( Entity );
		DECL_CONST( Arrow );
		DECL_CONST( Scene );
        DECL_CONST( Layer2D );
		DECL_CONST( Layer2DParallax );
		DECL_CONST( Layer2DIsometric );
		DECL_CONST( HotSpotPolygon );
		DECL_CONST( HotSpotBubbles );
        DECL_CONST( HotSpotImage );
        DECL_CONST( HotSpotShape );
        DECL_CONST( Point );
		DECL_CONST( Line );
		DECL_CONST( Window );
		DECL_CONST( Image );
		DECL_CONST( Video );
		DECL_CONST( MovieInternalObject );
		DECL_CONST( Sound );
		DECL_CONST( Movie );
        DECL_CONST( SubMovie );
		DECL_CONST( user );
		DECL_CONST( WhitePixel );
        DECL_CONST( Resource );
		DECL_CONST( ResourceSound );
		DECL_CONST( ResourceImageInAtlasDefault );
		DECL_CONST( ResourceImageInAtlasCombineRGBAndAlpha );
		DECL_CONST( ResourceImageSolid );
		DECL_CONST( ResourceImageDefault );
		DECL_CONST( ResourceImageDynamic );
		DECL_CONST( ResourceHotspotImage );
		DECL_CONST( ResourceImage );
		DECL_CONST( ResourceAnimation );
		DECL_CONST( ResourceInternalObject );
		DECL_CONST( ResourceMovie );
        DECL_CONST( ResourceHIT );
		//DECL_CONST( ResourceSequence );
		DECL_CONST( ResourceVideo );
		DECL_CONST( CreateImage );
		DECL_CONST( CreateTarget );
		DECL_CONST( CreateTexture );
		DECL_CONST( Sprite );
		DECL_CONST( Mesh2D );
		DECL_CONST( Grid2D );
		DECL_CONST( Animation );
		DECL_CONST( RenderViewport );
		DECL_CONST( RenderClipplane );
		DECL_CONST( Camera2D );
		DECL_CONST( CameraIsometric );
		DECL_CONST( Camera3D );
		DECL_CONST( TextField );
		DECL_CONST( __CONSOLE_FONT__ );		
		DECL_CONST( SoundEmitter );
		DECL_CONST( RigidBody2D );
		DECL_CONST( WorldPhysObject );
		DECL_CONST( NullTexture );

		DECL_CONST( Debug );

		//DECL_CONST( Blend );
		//DECL_CONST( SolidSprite );
		//DECL_CONST( OnlyColor );
		//DECL_CONST( Intensive_OnlyColor );
		//DECL_CONST( ExternalAlpha );
  //      DECL_CONST( ExternalAlpha_OnlyColor );
		//DECL_CONST( ExternalAlphaIntensive );
  //      DECL_CONST( ExternalAlphaIntensive_OnlyColor );
		//DECL_CONST( Debug );
		//DECL_CONST( Add );
		//DECL_CONST( Accumulator );
		//DECL_CONST( ColorAdd );
		//DECL_CONST( ColorBlend );
		//DECL_CONST( ColorSolid );

		DECL_CONST( ParticleEmitter );
		DECL_CONST( ParticleEmitter2 );

		DECL_CONST( LayerCamera2D );

		DECL_CONST( MovieSlot );
        DECL_CONST( MovieSocketImage );
        DECL_CONST( MovieSocketShape );
        DECL_CONST( MovieSceneEffect );
        DECL_CONST( MovieText );
		DECL_CONST( MovieTextCenter );
		DECL_CONST( MovieSprite );
		DECL_CONST( MovieNullObject );		
        DECL_CONST( MovieEvent );
		DECL_CONST( SolidSprite );
		DECL_CONST( SoundId );

		DECL_CONST( BlendingModeScreen );
		DECL_CONST( BlendingModeMultiply );
		DECL_CONST( BlendingModeAdd );
		DECL_CONST( BlendingModeNormal );

        DECL_CONST( AccountEnumerator );
        DECL_CONST( DefaultAccountID );
        DECL_CONST( SelectAccountID );
        DECL_CONST( Account );

		DECL_CONST( binMovie );
		DECL_CONST( aekMovie );
		DECL_CONST( xmlToAekMovie );

		DECL_CONST( Generic );
		DECL_CONST( Fade );

		DECL_CONST( memoryImage );
		DECL_CONST( archiveImage );

#	undef DECL_CONST
	};

#   define CONSTS_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::Consts)
}

#	define CONST_STRING( ServiceProvider, String )\
	(CONSTS_SERVICE(ServiceProvider)->c_##String)

