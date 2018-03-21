#include "Consts.h"

#include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Consts, Mengine::Consts );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Consts::Consts()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Consts::_initialize()
    {
#	define INIT_CONST( Const ) \
		c_##Const = STRINGIZE_STRING_LOCAL(#Const)

        INIT_CONST( dir );
        INIT_CONST( pak );
        INIT_CONST( zip );
        INIT_CONST( memory );
        INIT_CONST( eng );

        INIT_CONST( Default );
        INIT_CONST( Main );
        INIT_CONST( Homeless );
        INIT_CONST( Node );
		INIT_CONST( Node );
		INIT_CONST( Surface );
        INIT_CONST( Game );
        INIT_CONST( Entity );
        INIT_CONST( Arrow );
        INIT_CONST( Scene );
        INIT_CONST( Layer2D );
		INIT_CONST( Layer2DParallax );
		INIT_CONST( Layer2DIsometric );
		INIT_CONST( HotSpotPolygon );
		INIT_CONST( HotSpotBubbles );
        INIT_CONST( HotSpotImage );
        INIT_CONST( HotSpotShape );
        INIT_CONST( Point );
		INIT_CONST( Line );
        INIT_CONST( Window );
        INIT_CONST( Image );
        INIT_CONST( Video );
        INIT_CONST( MovieInternalObject );
        INIT_CONST( Sound );
        INIT_CONST( Movie );
        INIT_CONST( SubMovie );
        INIT_CONST( user );
        INIT_CONST( WhitePixel );
        INIT_CONST( Resource );
        INIT_CONST( ResourceSound );
        INIT_CONST( ResourceImageInAtlasCombineRGBAndAlpha );
        INIT_CONST( ResourceImageInAtlasDefault );
        INIT_CONST( ResourceImageSolid );
        INIT_CONST( ResourceImageDefault );
        INIT_CONST( ResourceImageDynamic );
        INIT_CONST( ResourceHotspotImage );
        INIT_CONST( ResourceImage );
        INIT_CONST( ResourceAnimation );
        INIT_CONST( ResourceInternalObject );
        INIT_CONST( ResourceMovie );
        INIT_CONST( ResourceHIT );
            //, c_ResourceSequence = Helper::stringizeString("ResourceSequence");
        INIT_CONST( ResourceVideo );
		INIT_CONST( SurfaceVideo );
		INIT_CONST( SurfaceSound );
        INIT_CONST( SurfaceImage );
        INIT_CONST( SurfaceSolidColor );
		INIT_CONST( SurfaceImageSequence );
        INIT_CONST( CreateImage );
        INIT_CONST( CreateTarget );
        INIT_CONST( CreateTexture );
        INIT_CONST( ShapeQuadFixed );
		INIT_CONST( Mesh2D );
		INIT_CONST( Grid2D );
        INIT_CONST( Animation );
		INIT_CONST( RenderCameraOrthogonal );
		INIT_CONST( RenderCameraProjection );
		INIT_CONST( RenderViewport );
		INIT_CONST( RenderClipplane );
        INIT_CONST( TextField );
        INIT_CONST( __CONSOLE_FONT__ );
        INIT_CONST( SoundEmitter );
        INIT_CONST( RigidBody2D );
        INIT_CONST( WorldPhysObject );
        INIT_CONST( NullTexture );

		INIT_CONST( Debug );

        INIT_CONST( ParticleEmitter );
		INIT_CONST( ParticleEmitter2 );
        INIT_CONST( LayerCamera2D );
        INIT_CONST( MovieSlot );
        INIT_CONST( MovieSocketImage );
        INIT_CONST( MovieSocketShape );
        INIT_CONST( MovieText );
		INIT_CONST( MovieTextCenter );
		INIT_CONST( MovieSprite );
        INIT_CONST( MovieSceneEffect );
        INIT_CONST( MovieNullObject );
		INIT_CONST( MovieParticle );
        INIT_CONST( MovieEvent );
		INIT_CONST( SolidSprite );
		INIT_CONST( SoundId );

		INIT_CONST( BlendingModeScreen );
		INIT_CONST( BlendingModeMultiply );
        INIT_CONST( BlendingModeAdd );
        INIT_CONST( BlendingModeNormal );

        INIT_CONST( AccountEnumerator );
        INIT_CONST( DefaultAccountID );
        INIT_CONST( SelectAccountID );
        INIT_CONST( Account );
		INIT_CONST( binMovie );
		INIT_CONST( aekMovie );
		INIT_CONST( xmlToAekMovie );
		
		INIT_CONST( Generic );
		INIT_CONST( Fade );

		INIT_CONST( memoryImage );
		INIT_CONST( archiveImage );

		INIT_CONST( ogvVideo );
		INIT_CONST( ogvaVideo );
		INIT_CONST( oggSound );

#	undef INIT_CONST

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Consts::_finalize()
	{
		//Empty
	}

}