#	include "Consts.h"

#   include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Consts, Menge::Consts, Menge::Consts );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Consts::Consts()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Consts::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Consts::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Consts::initialize()
    {
#	define INIT_CONST( Const ) c_##Const = Helper::stringizeString(m_serviceProvider, #Const )

        INIT_CONST( dir );
        INIT_CONST( pak );
        INIT_CONST( zip );
        INIT_CONST( memory );
        INIT_CONST( eng );

        INIT_CONST( Default );
        INIT_CONST( Main );
        INIT_CONST( Homeless );
        INIT_CONST( Node );
        INIT_CONST( Game );
        INIT_CONST( Entity );
        INIT_CONST( Arrow );
        INIT_CONST( Scene );
        INIT_CONST( Layer2D );
        INIT_CONST( HotSpot );
        INIT_CONST( HotSpotImage );
        INIT_CONST( HotSpotShape );
        INIT_CONST( Point );
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
        INIT_CONST( ResourceImageCombineRGBAndAlpha );
        INIT_CONST( ResourceImageDynamic );
        INIT_CONST( ResourceHotspotImage );
        INIT_CONST( ResourceImage );
        INIT_CONST( ResourceAnimation );
        INIT_CONST( ResourceInternalObject );
        INIT_CONST( ResourceMovie );
        INIT_CONST( ResourceHIT );
            //, c_ResourceSequence = Helper::StringizeString(m_serviceProvider, "ResourceSequence");
        INIT_CONST( ResourceVideo );
        INIT_CONST( ResourceEmitterContainer );
        INIT_CONST( CreateImage );
        INIT_CONST( CreateTarget );
        INIT_CONST( CreateTexture );
        INIT_CONST( Sprite );
		INIT_CONST( Mesh );
        INIT_CONST( Animation );
        INIT_CONST( Camera2D );
		INIT_CONST( RenderViewport );
        INIT_CONST( Camera3D );
        INIT_CONST( TextField );
        INIT_CONST( ConsoleFont );
        INIT_CONST( SoundEmitter );
        INIT_CONST( RigidBody2D );
        INIT_CONST( WorldPhysObject );
        INIT_CONST( NullTexture );
        INIT_CONST( BlendSprite );
        INIT_CONST( SolidSprite );
        INIT_CONST( OnlyColor );
        INIT_CONST( ExternalAlpha );
        INIT_CONST( ExternalAlpha_OnlyColor );
        INIT_CONST( Debug );
        INIT_CONST( ParticleIntensive );
        INIT_CONST( ParticleBlend );
        INIT_CONST( Accumulator );
        INIT_CONST( ParticleEmitter );
        INIT_CONST( LayerCamera2D );
        INIT_CONST( MovieSlot );
        INIT_CONST( MovieSocketImage );
        INIT_CONST( MovieSocketShape );
        INIT_CONST( MovieText );
		INIT_CONST( MovieTextCenter );
        INIT_CONST( MovieSceneEffect );
        INIT_CONST( MovieNullObject );
        INIT_CONST( MovieEvent );
        INIT_CONST( BlendingModeAdd );
        INIT_CONST( BlendingModeNormal );

        INIT_CONST( AccountEnumerator );
        INIT_CONST( DefaultAccountID );
        INIT_CONST( SelectAccountID );
        INIT_CONST( Account );
		INIT_CONST( binMovie );
		INIT_CONST( aekMovie );
		INIT_CONST( binToAekMovie );
		
		INIT_CONST( Generic );
		INIT_CONST( Fade );

        return true;
    }
}