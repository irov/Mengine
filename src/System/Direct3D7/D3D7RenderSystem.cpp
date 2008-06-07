
//#include <malloc.h>
#include "D3D7RenderSystem.h"
#include "DDDriver.h"
#include "D3D7Device.h"
#include "D3D7DeviceList.h"
#include "DDDriverList.h"
#include "DDVideoModeList.h"
#include "DDVideoMode.h"
//#include "RenderWindow.h"
//#include "LogManager.h"
//#include "RenderSystem.h"
//#include "Exception.h"
//#include "SceneManager.h"
//#include "D3D7TextureManager.h"
//#include "Viewport.h"
//#include "Light.h"
//#include "Matrix4.h"
//#include "Math.h"
#include "D3D7RenderWindow.h"
//#include "Frustum.h"
//#include "D3D7GpuProgramManager.h"
//#include "StringConverter.h"

#	include <sstream>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new Menge::D3DRenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrInterface )
{
	delete static_cast<Menge::D3DRenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
namespace Menge 
{
   /* const Matrix4 PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE(
        0.5,    0,  0, -0.5, 
          0, -0.5,  0, -0.5, 
          0,    0,  0,   -1,
          0,    0,  0,    1);

    const Matrix4 PROJECTIONCLIPSPACE2DTOIMAGESPACE_ORTHO(
        -0.5,   0,  0, -0.5, 
           0, 0.5,  0, -0.5, 
           0,   0,  0,   -1,
           0,   0,  0,    1);*/

    //////////////////////////////////////////////////////////////////////////
    D3DRenderSystem::D3DRenderSystem()
		: m_textureManager( NULL )
		, mlpD3DDevice( NULL )
		, m_driverList( NULL )
		, m_activeDDDriver( NULL )
		, m_deviceLost( false )
		, m_currentLights( 0 )
		, m_initialized( false )
   {
        // Reset driver list
        //m_hInstance = _hInstance;
        //mHardwareBufferManager = NULL;
       // mGpuProgramManager = NULL;
	   // set stages desc. to defaults
	   for ( int n = 0; n < MENGE_MAX_TEXTURE_LAYERS; n++ )
	   {
		   m_texStageDesc[n].autoTexCoordType = TEXCALC_NONE;
		   m_texStageDesc[n].coordIndex = 0;
		   m_texStageDesc[n].texType = D3D_TEX_TYPE_NORMAL;
		   m_texStageDesc[n].pTex = NULL;
	   }

	   m_eventNames.push_back("DeviceLost");
	   m_eventNames.push_back("DeviceRestored");

    }
    //////////////////////////////////////////////////////////////////////////
    D3DRenderSystem::~D3DRenderSystem()
    {
        shutdown();

        //SAFE_DELETE(m_textureManager);
        SAFE_DELETE(m_driverList);
       // SAFE_DELETE(mHardwareBufferManager);
        //SAFE_DELETE(mGpuProgramManager);


		if( m_initialized )
		{
			D3DXUninitialize();
		}
		m_logInterface->logMessage( String("D3D7: " + getName() + " destroyed.").c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    const String& D3DRenderSystem::getName() const
    {
        static String strName("Direct3D7 Rendering Subsystem");
        return strName;
    }
    //////////////////////////////////////////////////////////////////////////
    void D3DRenderSystem::initConfigOptions()
    {
        DDDriverList* ddList;
        DDDriver* dd;

        TConfigOption optDevice;
        TConfigOption optVideoMode;
        TConfigOption optFullScreen;
        TConfigOption optVSync;

        ddList = this->getDirectDrawDrivers();

        // Create option for devices
        optDevice.name = "Rendering Device";
        optDevice.currentValue = "";
        optDevice.possibleValues.clear();
        optDevice.immutable = false;

        // Option for video modes
        optVideoMode.name = "Video Mode";
        optVideoMode.currentValue = "";
        optVideoMode.immutable = false;

        // Option for full screen
        optFullScreen.name = "Full Screen";
        optFullScreen.possibleValues.push_back("Yes");
        optFullScreen.possibleValues.push_back("No");
        optFullScreen.currentValue = "Yes";

		std::size_t k = ddList->count();
        // First, get DirectDraw driver options
        for( unsigned j = 0; j < ddList->count(); j++ )
        {
            dd = ddList->item(j);
            // Add to device option list
            optDevice.possibleValues.push_back( dd->driverDescription() );

            // Make first one default
            if( j==0 )
            {
                optDevice.currentValue = dd->driverDescription();
            }
        }

        // VSync option
        optVSync.name = "VSync";
        optVSync.immutable = false;
        optVSync.possibleValues.push_back("Yes");
        optVSync.possibleValues.push_back("No");
        optVSync.currentValue = "Yes";

        m_options[optDevice.name] = optDevice;
        m_options[optVideoMode.name] = optVideoMode;
        m_options[optFullScreen.name] = optFullScreen;
        m_options[optVSync.name] = optVSync;

        // Set default-based settings
        refreshDDSettings();
    }
    //////////////////////////////////////////////////////////////////////////
    void D3DRenderSystem::refreshDDSettings()
    {
        DDVideoMode* vid;
        TConfigOption* optVideoMode;
        TConfigOption* optFullScreen;
        DDDriver* dd;

        // Stuffs DD-Driver specific settings

        // Find DD Driver selected in options
        TConfigOptionMap::iterator opt = m_options.find( "Rendering Device" );

        if( opt != m_options.end() )
        {
            for( unsigned j = 0; j < getDirectDrawDrivers()->count(); j++ )
            {
                dd = getDirectDrawDrivers()->item(j);
                if( dd->driverDescription() == opt->second.currentValue )
				{
                    break;
				}
            }

            // Get fullScreen and Video mode options
            opt = m_options.find( "Video Mode" );
            optVideoMode = &opt->second;
            opt = m_options.find( "Full Screen" );
            optFullScreen = &opt->second;

            // Full screen forced?
            if( !( dd->canRenderWindowed() ) )
            {
                setConfigOption( "Full Screen", "Yes" );
                optFullScreen->immutable = true;
            }
            else
			{
                optFullScreen->immutable = false;
			}

            // Get video modes for this device
            optVideoMode->possibleValues.clear();

            for( unsigned k = 0; k < dd->getVideoModeList()->count(); k++ )
            {
                vid = dd->getVideoModeList()->item( k );
                optVideoMode->possibleValues.push_back( vid->Description() );
            }
        }
    }
 //   //////////////////////////////////////////////////////////////////////////
 //   TConfigOptionMap& D3DRenderSystem::getConfigOptions()
 //   {
 //       // Return a COPY of the current config options
 //       return m_options;
 //   }
    //////////////////////////////////////////////////////////////////////////
    void D3DRenderSystem::setConfigOption( const String& _name, const String& _value )
    {
        std::ostringstream str;
        str << "RenderSystem Option: " << _name << " = " << _value;
        m_logInterface->logMessage( str.str() );

        // Find option
        TConfigOptionMap::iterator it = m_options.find( _name );

        // Update
        if( it != m_options.end() )
		{
            it->second.currentValue = _value;
		}
        else
        {
            str.clear();
            str << "Option named " << _name << " does not exist.";
            assert( 0 && String( str.str() + "D3DRenderSystem::setConfigOption" ).c_str() );
        }

        // Refresh other options if DD Driver changed
        if( _name == "Rendering Device" )
		{
            refreshDDSettings();
		}

        if( _name == "Full Screen" )
        {
            if( _value == "No" )
            {
                // Video mode is not applicable
                it = m_options.find( "Video Mode" );
                it->second.currentValue = "N/A";
                it->second.immutable = true;
            }
            else
            {
                // Video mode is applicable
                it = m_options.find( "Video Mode" );
                // default to 640 x 480 x 16
                it->second.currentValue = "640 x 480 @ 16-bit colour";
                it->second.immutable = false;
            }
        }
		if( _name == "VSync" )
		{
			if ( _value == "Yes" )
			{
				m_VSync = true;
			}
			else
			{
				m_VSync = false;
			}
		}
    }
 //   //////////////////////////////////////////////////////////////////////////
 //   String D3DRenderSystem::validateConfigOptions()
 //   {
 //       // Check video mode specified in full screen mode
 //       TConfigOptionMap::iterator o = m_options.find( "Full Screen" );
 //       if( o->second.currentValue == "Yes" )
 //       {
 //           // Check video mode
 //           o = m_options.find( "Video Mode" );
 //           if( o->second.currentValue == "" )
 //           {
 //               return "A video mode must be selected for running in full-screen mode.";
 //           }
 //       }

 //       o = m_options.find( "Rendering Device" );
 //       bool foundDriver = false;
 //       DDDriverList* driverList = getDirectDrawDrivers();
 //       for( unsigned short j = 0; j < driverList->count(); j++ )
 //       {
 //           if( driverList->item(j)->driverDescription() == o->second.currentValue )
 //           {
 //               foundDriver = true;
 //               break;
 //           }
 //       }
 //       if( !foundDriver )
 //       {
 //           // Just pick the first driver
 //           setConfigOption( "Rendering Device", driverList->item( 0 )->driverDescription() );
 //           return "Your DirectX driver name has changed since the last time you ran Menge Application; "
 //               "the 'Rendering Device' has been changed.";
 //       }
 //       return "";

 //   }
    //////////////////////////////////////////////////////////////////////////
    void D3DRenderSystem::initialise( LogSystemInterface* _logInterface )
    {
		m_logInterface = _logInterface;
		m_logInterface->logMessage( String("D3D7: " + getName() + " created.").c_str() );

		if( !m_initialized )
		{
			initConfigOptions();

			// Initialise D3DX library
			D3DXInitialize();
		}


        m_logInterface->logMessage(
            "***************************************\n"
            "*** Direct3D Subsystem Initialising ***\n"
            "***************************************" );

        // ---------------------------
        // Init using current settings
        // ---------------------------

        // DirectDraw driver
        m_activeDDDriver = 0;
        TConfigOptionMap::iterator opt = m_options.find( "Rendering Device" );
        for( unsigned j = 0; j < getDirectDrawDrivers()->count(); j++ )
        {
            if ( getDirectDrawDrivers()->item( j )->driverDescription() == opt->second.currentValue )
            {
                m_activeDDDriver = getDirectDrawDrivers()->item( j );
                break;
            }
        }

        assert( m_activeDDDriver && "D3DRenderSystem::initialise -> Problems finding requested DirectDraw driver!" );

        opt = m_options.find( "VSync" );
        if( opt != m_options.end() )
        {
            if( opt->second.currentValue == "Yes" )
            {
                setWaitForVerticalBlank( true );
            }
            else
            {
                setWaitForVerticalBlank( false );
            }
        }

        m_logInterface->logMessage( "*****************************************" );
        m_logInterface->logMessage( "*** Direct3D Subsystem Initialised Ok ***" );
        m_logInterface->logMessage( "*****************************************" );

		m_vertexProgramBound = false;
		m_fragmentProgramBound = false;

		m_initialized = true;
		// Create buffer manager
        //m_hardwareBufferManager = new D3D7HardwareBufferManager();
        // Create dummy gpu manager
        // m_gpuProgramManager = new D3D7GpuProgramManager();
    }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::reinitialise()
 //   {
 //       this->shutdown();
 //       this->initialise();
 //   }
 //   //////////////////////////////////////////////////////////////////////////
    void D3DRenderSystem::shutdown()
    {

		// Remove all the render targets.
		// (destroy primary target last since others may depend on it)
		/*RenderTarget* primary = 0;
		for( TRenderTargetMap::iterator it = m_renderTargets.begin(), it_end = m_renderTargets.end();
			it != it_end; 
			++it )
		{
			if ( !primary && it->second->isPrimary() )
			{
				primary = it->second;
			}
			else
			{
				delete it->second;
			}
		}
		delete primary;
		m_renderTargets.clear();*/

		m_prioritisedRenderTargets.clear();

        // Cleanup
        // Release all DirectX resources
        //D3DTextr_InvalidateAllTextures();
        if( m_activeDDDriver )
        {
            m_activeDDDriver->cleanup();
        }

        // Delete system objects
        SAFE_DELETE( m_driverList );

        m_activeDDDriver = NULL;

        // Write termination message
        m_logInterface->logMessage( "*-*-* Direct3D Subsystem shutting down cleanly." );
    }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setAmbientLight( float _r, float _g, float _b )
 //   {
 //       // Call D3D
 //       D3DCOLOR col = D3DRGB(_r,_g,_b);

 //       HRESULT hr = __SetRenderState( D3DRENDERSTATE_AMBIENT, col );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::setAmbientLight -> Error setting ambient light." );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setShadingType( ShadeOptions _so )
 //   {
 //       D3DSHADEMODE d3dMode;
 //       switch(_so)
 //       {
 //       case SO_FLAT:
 //           d3dMode = D3DSHADE_FLAT;
 //           break;
 //       case SO_GOURAUD:
 //           d3dMode = D3DSHADE_GOURAUD;
 //           break;
 //       case SO_PHONG:
 //           d3dMode = D3DSHADE_PHONG;
 //           break;

 //       }

 //       HRESULT hr = __SetRenderState( D3DRENDERSTATE_SHADEMODE, d3dMode );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::setShadingType -> Error setting shading mode." );
 //   }
    //////////////////////////////////////////////////////////////////////////
	bool D3DRenderSystem::createRenderWindow( const String& _name, unsigned int _width, unsigned int _height,
		bool _fullScreen, const NameValuePairList* _miscParams )
    {
        static bool firstWindow = true;

        String msg;

        // Make sure we don't already have a render target of the
        // same name as the one supplied
        assert( m_renderTargets.find( _name ) == m_renderTargets.end() && 
			String( "D3DRenderSystem::createRenderWindow -> A render target of the same name (" + _name + ") already "
                "exists. You cannot create a new window with this name." ).c_str() );

        D3D7RenderWindow* win = new D3D7RenderWindow( m_activeDDDriver );
        // Create window, supplying DD interface & hInstance
        win->create( _name, _width, _height, _fullScreen, _miscParams );
		
        //attachRenderTarget( *win );

        // If this is the first window, get the D3D device
        //  and create the texture manager, setup caps
        if( firstWindow )
        {
            win->getCustomAttribute( "D3DDEVICE", &mlpD3DDevice );
            // Get caps
            mlpD3DDevice->GetCaps( &m_D3DDeviceDesc );
            // Create my texture manager for use by others
            // Note this is a Singleton; pointer is held static by superclass
           // m_textureManager = new D3DTextureManager( mlpD3DDevice );

            // Check for hardware stencil support
            // Get render target, then depth buffer and check format
            LPDIRECTDRAWSURFACE7 lpTarget;
            win->getCustomAttribute( "DDBACKBUFFER", &lpTarget );
            DDSCAPS2 ddscaps;
            ZeroMemory( &ddscaps, sizeof( DDSCAPS2 ) );
            ddscaps.dwCaps = DDSCAPS_ZBUFFER;
            lpTarget->GetAttachedSurface( &ddscaps, &lpTarget );
            lpTarget->Release();
            DDSURFACEDESC2 ddsd;
            ddsd.dwSize = sizeof( DDSURFACEDESC2 );
            lpTarget->GetSurfaceDesc( &ddsd );
            DWORD stencil =  ddsd.ddpfPixelFormat.dwStencilBitDepth;
            if( stencil > 0 )
            {
               /* m_capabilities->setCapability( RSC_HWSTENCIL );
                m_capabilities->setStencilBufferBitDepth( stencil );
                if ( ( m_D3DDeviceDesc.dwStencilCaps & D3DSTENCILCAPS_INCR ) && 
                    ( m_D3DDeviceDesc.dwStencilCaps & D3DSTENCILCAPS_DECR ) )
				{
                    m_capabilities->setCapability( RSC_STENCIL_WRAP );
				}*/
            }

            // Anisotropy?
            if( m_D3DDeviceDesc.dwMaxAnisotropy > 1 )
			{
               // m_capabilities->setCapability( RSC_ANISOTROPY );
			}
            // Blending between stages supported
            //m_capabilities->setCapability(RSC_BLENDING);
            // Cubemapping
            if( m_D3DDeviceDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP )
			{
                //m_capabilities->setCapability( RSC_CUBEMAPPING );
			}
            // DOT3
            if( m_D3DDeviceDesc.dwTextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3 )
			{
               // m_capabilities->setCapability( RSC_DOT3 );
			}
            // Set the number of texture units based on details from current device
           // m_capabilities->setNumTextureUnits( m_D3DDeviceDesc.wMaxSimultaneousTextures );

           /* Log* defaultLog = LogManager::getSingleton().getDefaultLog();
			if (defaultLog)
			{
				m_capabilities->log(defaultLog);
			}*/

            firstWindow = false;
        }

		return true;
    }
	////////////////////////////////////////////////////////////////////////////
	///*MultiRenderTarget * D3DRenderSystem::createMultiRenderTarget( const String& _name )
	//{
	//	OGRE_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "", "" );
	//	return NULL;
	//}*/
	////////////////////////////////////////////////////////////////////////////
	//RenderTexture * D3DRenderSystem::createRenderTexture( const String& _name, unsigned int _width, unsigned int _height,
	//	TextureType _texType, PixelFormat _internalFormat, const NameValuePairList* _miscParams ) 
 //   {
	//	/// Create a new 2D texture, and return surface to render to
	//	TexturePtr texture = TextureManager::getSingleton().createManual( name, 
	//		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, texType, 
	//		width, height, 0, internalFormat, TU_RENDERTARGET );

	//	// Ensure texture loaded and internal resources created
	//	texture->load();

	//	return texture->getBuffer()->getRenderTarget();
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   //-----------------------------------------------------------------------
 //   // Low-level overridden members
 //   //-----------------------------------------------------------------------
	////---------------------------------------------------------------------
 //   void D3DRenderSystem::_useLights( const TLightList& _lights, unsigned short _limit )
 //   {
 //       TLightList::const_iterator i, iend;
 //       iend = lights.end();
 //       unsigned short num = 0;
 //       for( i = _lights.begin(); i != iend && num < _limit; ++i, ++num )
 //       {
 //           setD3DLight( num, *i );
 //       }
 //       // Disable extra lights
 //       for ( ; num < m_currentLights; ++num )
 //       {
 //           setD3DLight( num, NULL );
 //       }
 //       m_currentLights = (std::min)( _limit, static_cast<unsigned short>( _lights.size() ) );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::setD3DLight( std::size_t _index, Light* _lt )
 //   {
 //       // Add to D3D
 //       HRESULT hr;
 //       D3DLIGHT7 d3dLight;

 //       if ( !_lt )
 //       {
 //           hr = mlpD3DDevice->LightEnable( _index, FALSE );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::setD3DLight -> Unable to disable light." );
 //       }
 //       else
 //       {
 //           switch ( _lt->getType() )
 //           {
 //           case Light::LT_POINT:
 //               d3dLight.dltType = D3DLIGHT_POINT;
 //               break;
 //           case Light::LT_DIRECTIONAL:
 //               d3dLight.dltType = D3DLIGHT_DIRECTIONAL;
 //               break;
 //           case Light::LT_SPOTLIGHT:
 //               d3dLight.dltType = D3DLIGHT_SPOT;
 //               d3dLight.dvFalloff = lt->getSpotlightFalloff();
 //               d3dLight.dvTheta = lt->getSpotlightInnerAngle().valueRadians();
 //               d3dLight.dvPhi = lt->getSpotlightOuterAngle().valueRadians();
 //               break;
 //           }

 //           // Colours
 //           ColourValue col;
 //           col = _lt->getDiffuseColour();
 //           d3dLight.dcvDiffuse.r = col.r;
 //           d3dLight.dcvDiffuse.g = col.g;
 //           d3dLight.dcvDiffuse.b = col.b;
 //           d3dLight.dcvDiffuse.a = col.a;

 //           col = _lt->getSpecularColour();
 //           d3dLight.dcvSpecular.r = col.r;
 //           d3dLight.dcvSpecular.g = col.g;
 //           d3dLight.dcvSpecular.b = col.b;
 //           d3dLight.dcvSpecular.a = col.a;

 //           // Never use ambient for a movable light
 //           d3dLight.dcvAmbient.r = 0.0;
 //           d3dLight.dcvAmbient.g = 0.0;
 //           d3dLight.dcvAmbient.b = 0.0;
 //           d3dLight.dcvAmbient.a = 0.0;

 //           // Position (Irrelevant for directional)
 //           Vector3 vec;
 //           if( _lt->getType() != Light::LT_DIRECTIONAL)
 //           {
 //               vec = _lt->getDerivedPosition();

 //               d3dLight.dvPosition.x = vec.x;
 //               d3dLight.dvPosition.y = vec.y;
 //               d3dLight.dvPosition.z = vec.z;
 //           }
 //           // Direction (Irrelevant for point lights)
 //           if( _lt->getType() != Light::LT_POINT)
 //           {
 //               vec = _lt->getDerivedDirection();
 //               d3dLight.dvDirection.x = vec.x;
 //               d3dLight.dvDirection.y = vec.y;
 //               d3dLight.dvDirection.z = vec.z;
 //           }
 //           // Attenuation parameters
 //           d3dLight.dvRange = _lt->getAttenuationRange();
 //           d3dLight.dvAttenuation0 = _lt->getAttenuationConstant();
 //           d3dLight.dvAttenuation1 = _lt->getAttenuationLinear();
 //           d3dLight.dvAttenuation2 = _lt->getAttenuationQuadric();

 //           // Set light state
 //           hr = mlpD3DDevice->SetLight( _index, &d3dLight );

 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::setD3DLight -> Unable to set light details" );

 //           hr = mlpD3DDevice->LightEnable( _index, TRUE );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::setD3DLight -> Unable to enable light." );
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   D3DMATRIX D3DRenderSystem::makeD3DMatrix( const float* _mat )
 //   {
 //       // Transpose matrix
 //       // D3D & Menge uses row vectors i.e. V*M
 //       D3DMATRIX d3dMat;

 //       d3dMat.m[0][0] = _mat[0][0];
 //       d3dMat.m[0][1] = _mat[0][1];
 //       d3dMat.m[0][2] = _mat[0][2];
 //       d3dMat.m[0][3] = _mat[0][3];

 //       d3dMat.m[1][0] = _mat[1][0];
 //       d3dMat.m[1][1] = _mat[1][1];
 //       d3dMat.m[1][2] = _mat[1][2];
 //       d3dMat.m[1][3] = _mat[1][3];

 //       d3dMat.m[2][0] = _mat[2][0];
 //       d3dMat.m[2][1] = _mat[2][1];
 //       d3dMat.m[2][2] = _mat[2][2];
 //       d3dMat.m[2][3] = _mat[2][3];

 //       d3dMat.m[3][0] = _mat[3][0];
 //       d3dMat.m[3][1] = _mat[3][1];
 //       d3dMat.m[3][2] = _mat[3][2];
 //       d3dMat.m[3][3] = _mat[3][3];

 //       return d3dMat;
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   float* D3DRenderSystem::convertD3DMatrix( const D3DMATRIX& _d3dmat )
 //   {
 //       // The reverse of makeD3DMatrix
 //       // Transpose matrix
	//	// D3D & Menge uses row vectors i.e. V*M

	//	Matrix4 mat;
 //       for ( unsigned row = 0; row < 4; ++row )
	//	{
 //           for ( unsigned col = 0; col < 4; ++col )
	//		{
 //               mat[col][row] = d3dmat.m[col][row];
	//		}
	//	}

 //       return mat;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setWorldMatrix( const float* _m )
 //   {
 //       D3DMATRIX d3dmat;

 //       d3dmat = makeD3DMatrix( _m );

 //       HRESULT hr = mlpD3DDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &d3dmat );

 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setWorldMatrix -> Cannot set D3D world matrix" );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setViewMatrix( const float* _m )
 //   {
 //       // save latest view matrix
 //       m_viewMatrix = _m;
 //       m_viewMatrix[2][0] = -m_viewMatrix[2][0];
 //       m_viewMatrix[2][1] = -m_viewMatrix[2][1];
 //       m_viewMatrix[2][2] = -m_viewMatrix[2][2];
 //       m_viewMatrix[2][3] = -m_viewMatrix[2][3];

 //       D3DMATRIX d3dmat = makeD3DMatrix( m_viewMatrix );

 //       HRESULT hr = mlpD3DDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &d3dmat );

 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setViewMatrix -> Cannot set D3D view matrix" );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setProjectionMatrix( const float* _m )
 //   {
 //       D3DMATRIX d3dmat = makeD3DMatrix( _m );

 //       if( m_activeRenderTarget->requiresTextureFlipping() )
 //       {
 //           d3dmat._22 = - d3dmat._22;
 //       }

 //       HRESULT hr = mlpD3DDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &d3dmat );

 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setProjectionMatrix -> Cannot set D3D projection matrix" );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setSurfaceParams( const ColourValue& _ambient, const ColourValue& _diffuse,
	//	const ColourValue& _specular, const ColourValue& _emissive, const float _shininess, int _tracking )
 //   {
 //       // Remember last call
 //       static ColourValue lastAmbient = ColourValue::Black;
 //       static ColourValue lastDiffuse = ColourValue::Black;
 //       static ColourValue lastSpecular = ColourValue::Black;
 //       static ColourValue lastEmissive = ColourValue::Black;
 //       static float lastShininess = 0.0;
 //       static TrackVertexColourType lastTracking = -1;

 //       // Only update if changed
 //       if( _ambient != lastAmbient || _diffuse != lastDiffuse ||
 //           _specular != lastSpecular || _emissive != lastEmissive ||
 //           _shininess != lastShininess )
 //       {
 //           // Convert to D3D
 //           D3DMATERIAL7 d3dMat;

 //           d3dMat.dcvDiffuse.r = _diffuse.r;
 //           d3dMat.dcvDiffuse.g = _diffuse.g;
 //           d3dMat.dcvDiffuse.b = _diffuse.b;
 //           d3dMat.dcvDiffuse.a = _diffuse.a;

 //           d3dMat.dcvAmbient.r = _ambient.r;
 //           d3dMat.dcvAmbient.g = _ambient.g;
 //           d3dMat.dcvAmbient.b = _ambient.b;
 //           d3dMat.dcvAmbient.a = _ambient.a;

 //           d3dMat.dcvSpecular.r = _specular.r;
 //           d3dMat.dcvSpecular.g = _specular.g;
 //           d3dMat.dcvSpecular.b = _specular.b;
 //           d3dMat.dcvSpecular.a = _specular.a;

 //           d3dMat.dcvEmissive.r = _emissive.r;
 //           d3dMat.dcvEmissive.g = _emissive.g;
 //           d3dMat.dcvEmissive.b = _emissive.b;
 //           d3dMat.dcvEmissive.a = _emissive.a;

 //           d3dMat.dvPower = _shininess;

 //           HRESULT hr = mlpD3DDevice->SetMaterial( &d3dMat );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_setSurfaceParams -> Error setting D3D material." );

 //           // Remember the details
 //           lastAmbient = _ambient;
 //           lastDiffuse = _diffuse;
 //           lastSpecular = _specular;
 //           lastEmissive = _emissive;
 //           lastShininess = _shininess;                
 //       }
 //       if( _tracking != lastTracking ) 
 //       {
 //           if( _tracking != TVC_NONE ) 
 //           {
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_COLORVERTEX, TRUE );
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_AMBIENTMATERIALSOURCE, (tracking&TVC_AMBIENT)?D3DMCS_COLOR1:D3DMCS_MATERIAL );
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, (tracking&TVC_DIFFUSE)?D3DMCS_COLOR1:D3DMCS_MATERIAL );
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_SPECULARMATERIALSOURCE, (tracking&TVC_SPECULAR)?D3DMCS_COLOR1:D3DMCS_MATERIAL );
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, (tracking&TVC_EMISSIVE)?D3DMCS_COLOR1:D3DMCS_MATERIAL );
 //           } 
 //           else 
 //           {
 //               mlpD3DDevice->SetRenderState( D3DRENDERSTATE_COLORVERTEX, FALSE );
 //           }
 //           lastTracking = _tracking;
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTexture( std::size_t _stage, bool _enabled, const String& _texname )
 //   {
 //       HRESULT hr;
 //       D3DTexturePtr dt = TextureManager::getSingleton().getByName( _texname );
 //       if ( _enabled && !dt.isNull() )
 //       {
 //           // note used
 //           dt->touch();

 //           LPDIRECTDRAWSURFACE7 pTex = dt->getDDSurface();
 //           if (pTex != m_texStageDesc[stage].pTex)
 //           {
 //               hr = mlpD3DDevice->SetTexture(stage, pTex );
 //               assert( SUCCEEDED(hr) && "D3DRenderSystem::_setTexture -> Unable to set texture in D3D." );

 //               // set stage desc.
 //               m_texStageDesc[stage].texType = _ogreTexTypeToD3DTexType(dt->getTextureType());
 //               m_texStageDesc[stage].pTex = pTex;
 //           }
 //       }
 //       else
 //       {
 //           hr = mlpD3DDevice->SetTexture( _stage, 0 );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_setTexture -> Unable to disable texture in D3D." );
 //           hr = __SetTextureStageState( _stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_setTexture -> Unable to disable texture in D3D." );

 //           // set stage desc. to defaults
 //           m_texStageDesc[_stage].autoTexCoordType = TEXCALC_NONE;
 //           m_texStageDesc[_stage].coordIndex = 0;
 //           m_texStageDesc[_stage].texType = D3D_TEX_TYPE_NORMAL;
 //           m_texStageDesc[_stage].pTex = NULL;
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   DWORD getD3DTexCalc( TexCoordCalcMethod _tcc )
 //   {
 //       switch (tcc)
 //       {
 //       case TEXCALC_NONE:
 //           return 0;
 //       case TEXCALC_ENVIRONMENT_MAP: 
 //           // D3D7 does not support spherical reflection
 //           return D3DTSS_TCI_CAMERASPACENORMAL;
 //       case TEXCALC_ENVIRONMENT_MAP_REFLECTION:
 //           return D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
 //       case TEXCALC_ENVIRONMENT_MAP_PLANAR:
 //       case TEXCALC_PROJECTIVE_TEXTURE:
 //           return D3DTSS_TCI_CAMERASPACEPOSITION;
 //       case TEXCALC_ENVIRONMENT_MAP_NORMAL:
 //           return D3DTSS_TCI_CAMERASPACENORMAL;
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureCoordCalculation( std::size_t _stage, TexCoordCalcMethod _m, 
 //       const Frustum* _frustum )
 //   {
 //       HRESULT hr = S_OK;
 //       // record the stage state
 //       m_texStageDesc[_stage].autoTexCoordType = _m;
 //       m_texStageDesc[_stage].frustum = _frustum;

 //       hr = __SetTextureStageState( _stage, D3DTSS_TEXCOORDINDEX, getD3DTexCalc(_m) | m_texStageDesc[_stage].coordIndex );
 //       assert( SUCCEEDED( hr ) && "D3DRenderSystem::_setTextureCoordCalculation -> Error setting texture coord calculation" );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureMatrix( std::size_t _stage, const float* _xForm )
 //   {
	//	HRESULT hr;
	//	D3DMATRIX d3dMat; // the matrix we'll maybe apply
	//	Matrix4 newMat = _xForm; // the matrix we'll apply after conv. to D3D format
 //       

	//	/* If envmap is applied, since D3D7 doesn't support spheremap,
	//	then we have to use texture transform to make the camera space normal
	//	reference the envmap properly. This isn't exactly the same as spheremap
	//	(it looks nasty on flat areas because the camera space normals are the same)
	//	but it's the best approximation we have in the absence of a proper spheremap */
	//	if( m_texStageDesc[_stage].autoTexCoordType == TEXCALC_ENVIRONMENT_MAP )
	//	{
	//		// concatenate with the xForm
 //           newMat = newMat.concatenate(Matrix4::CLIPSPACE2DTOIMAGESPACE);
	//	}

 //       // If this is a cubic reflection, we need to modify using the view matrix
 //       if( m_texStageDesc[_stage].autoTexCoordType == TEXCALC_ENVIRONMENT_MAP_REFLECTION )
 //       {
 //           D3DMATRIX viewMatrix; 

 //           // Get view matrix
 //           mlpD3DDevice->GetTransform( D3DTRANSFORMSTATE_VIEW, &viewMatrix );
 //           // Get transposed 3x3, ie since D3D is transposed just copy
 //           // We want to transpose since that will invert an orthonormal matrix ie rotation
 //           Matrix4 ogreViewTransposed;
 //           ogreViewTransposed[0][0] = viewMatrix.m[0][0];
 //           ogreViewTransposed[0][1] = viewMatrix.m[0][1];
 //           ogreViewTransposed[0][2] = viewMatrix.m[0][2];
 //           ogreViewTransposed[0][3] = 0.0f;

 //           ogreViewTransposed[1][0] = viewMatrix.m[1][0];
 //           ogreViewTransposed[1][1] = viewMatrix.m[1][1];
 //           ogreViewTransposed[1][2] = viewMatrix.m[1][2];
 //           ogreViewTransposed[1][3] = 0.0f;

 //           ogreViewTransposed[2][0] = viewMatrix.m[2][0];
 //           ogreViewTransposed[2][1] = viewMatrix.m[2][1];
 //           ogreViewTransposed[2][2] = viewMatrix.m[2][2];
 //           ogreViewTransposed[2][3] = 0.0f;

 //           ogreViewTransposed[3][0] = 0.0f;
 //           ogreViewTransposed[3][1] = 0.0f;
 //           ogreViewTransposed[3][2] = 0.0f;
 //           ogreViewTransposed[3][3] = 1.0f;
 //           
 //           newMat = newMat.concatenate(ogreViewTransposed);
 //       }

 //       if (m_texStageDesc[stage].autoTexCoordType == TEXCALC_PROJECTIVE_TEXTURE)
 //       {
 //           // Derive camera space to projector space transform
 //           // To do this, we need to undo the camera view matrix, then 
 //           // apply the projector view & projection matrices
 //           newMat = mViewMatrix.inverse();
 //           newMat = m_texStageDesc[stage].frustum->getViewMatrix() * newMat;
 //           newMat = m_texStageDesc[stage].frustum->getProjectionMatrix() * newMat;
 //           if (m_texStageDesc[stage].frustum->getProjectionType() == PT_PERSPECTIVE)
 //           {
 //               newMat = PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE * newMat;
 //           }
 //           else
 //           {
 //               newMat = PROJECTIONCLIPSPACE2DTOIMAGESPACE_ORTHO * newMat;
 //           }
 //           newMat = xForm * newMat;

 //       }

	//	// convert our matrix to D3D format
	//	d3dMat = makeD3DMatrix(newMat);

	//	// need this if texture is a cube map, to invert D3D's z coord
	//	if (m_texStageDesc[stage].autoTexCoordType != TEXCALC_NONE &&
 //           m_texStageDesc[stage].autoTexCoordType != TEXCALC_PROJECTIVE_TEXTURE)
	//	{
	//		d3dMat._13 = -d3dMat._13;
	//		d3dMat._23 = -d3dMat._23;
	//		d3dMat._33 = -d3dMat._33;
	//		d3dMat._43 = -d3dMat._43;
	//	}

	//	// set the matrix if it's not the identity
 //       if (!(newMat == Matrix4::IDENTITY))
	//	{
	//		// tell D3D the dimension of tex. coord.
	//		int texCoordDim;
 //           if (m_texStageDesc[stage].autoTexCoordType == TEXCALC_PROJECTIVE_TEXTURE)
 //           {
 //               texCoordDim = D3DTTFF_PROJECTED | D3DTTFF_COUNT3;
 //           }
 //           else
 //           {
	//		    switch (m_texStageDesc[stage].texType)
	//		    {
	//		    case D3D_TEX_TYPE_NORMAL:
	//			    texCoordDim = D3DTTFF_COUNT2;
	//			    break;
	//		    case D3D_TEX_TYPE_CUBE:
	//		    case D3D_TEX_TYPE_VOLUME:
	//			    texCoordDim = D3DTTFF_COUNT3;
	//		    }
 //           }

	//		hr = __SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, texCoordDim );
	//		assert( SUCCEEDED(hr) && "D3D7RenderSystem::_setTextureMatrix -> Unable to set texture coord. dimension" );

	//		hr = mlpD3DDevice->SetTransform( 
 //               (D3DTRANSFORMSTATETYPE)(D3DTRANSFORMSTATE_TEXTURE0 + _stage ), &d3dMat );
	//		assert( SUCCEEDED(hr) && "D3D7RenderSystem::_setTextureMatrix -> Unable to set texture matrix" );
	//	}
	//	else
	//	{
	//		// disable all of this
	//		hr = __SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	//		assert( SUCCEEDED( hr ) && "D3D7RenderSystem::_setTextureMatrix -> Error setting texture matrix" );

	//		// set the identity matrix
	//		D3DUtil_SetIdentityMatrix( d3dMat );
	//		hr = mlpD3DDevice->SetTransform( 
 //               (D3DTRANSFORMSTATETYPE)(D3DTRANSFORMSTATE_TEXTURE0 + _stage ), &d3dMat );
	//		assert( SUCCEEDED( hr ) && "D3D7RenderSystem::_setTextureMatrix -> Error setting texture matrix" );
	//	}
 //   }
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureCoordSet( size_t stage, size_t index )
	//{
	//	HRESULT hr;
	//	hr = __SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, 
 //           getD3DTexCalc(m_texStageDesc[stage].autoTexCoordType) | index );
	//	assert( SUCCEEDED( hr ) && "D3DRenderSystem::_setTextureCoordSet -> Unable to set texture coord. set index" );
 //       // Record settings
 //       m_texStageDesc[stage].coordIndex = index;
	//}
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setTextureBlendMode( size_t _stage, const LayerBlendModeEx& _bm )
 //   {
 //       HRESULT hr;
 //       D3DTEXTURESTAGESTATETYPE tss;
 //       DWORD value;

 //       if (_bm.blendType == LBT_COLOUR)
 //       {
 //           tss = D3DTSS_COLOROP;
 //       }
 //       else if (_bm.blendType == LBT_ALPHA)
 //       {
 //           tss= D3DTSS_ALPHAOP;
 //       }

 //       switch (_bm.operation)
 //       {
 //       case LBX_SOURCE1:
 //           value = D3DTOP_SELECTARG1;
 //           break;
 //       case LBX_SOURCE2:
 //           value = D3DTOP_SELECTARG2;
 //           break;
 //       case LBX_MODULATE:
 //           value = D3DTOP_MODULATE;
 //           break;
 //       case LBX_MODULATE_X2:
 //           value = D3DTOP_MODULATE2X;
 //           break;
 //       case LBX_MODULATE_X4:
 //           value = D3DTOP_MODULATE4X;
 //           break;
 //       case LBX_ADD:
 //           value = D3DTOP_ADD;
 //           break;
 //       case LBX_ADD_SIGNED:
 //           value = D3DTOP_ADDSIGNED;
 //           break;
 //       case LBX_ADD_SMOOTH:
 //           value = D3DTOP_ADDSMOOTH;
 //           break;
 //       case LBX_SUBTRACT:
 //           value = D3DTOP_SUBTRACT;
 //           break;
 //       case LBX_BLEND_DIFFUSE_ALPHA:
 //           value = D3DTOP_BLENDDIFFUSEALPHA;
 //           break;
 //       case LBX_BLEND_TEXTURE_ALPHA:
 //           value = D3DTOP_BLENDTEXTUREALPHA;
 //           break;
 //       case LBX_BLEND_CURRENT_ALPHA:
 //           value = D3DTOP_BLENDCURRENTALPHA;
 //           break;
 //       case LBX_BLEND_MANUAL:
 //           value = D3DTOP_BLENDFACTORALPHA;
 //           // Set factor in render state
 //           hr = __SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR,
 //               D3DRGBA(0,0,0,_bm.factor));
 //           break;
 //       case LBX_DOTPRODUCT:
 //           if (m_D3DDeviceDesc.dwTextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)
 //               value = D3DTOP_DOTPRODUCT3;
 //           else
 //               value = D3DTOP_MODULATE;
 //           break;
 //       }

 //       // Make call to set operation
 //       hr = __SetTextureStageState(_stage, tss, value);

 //       // Now set up sources
 //       D3DCOLOR manualD3D;
 //       if (_bm.blendType == LBT_COLOUR)
 //       {
 //           tss = D3DTSS_COLORARG1;
 //           manualD3D = D3DRGBA(_bm.colourArg1.r,_bm.colourArg1.g,_bm.colourArg1.b,1.0);
	//		mManualBlendColours[_stage][0] = _bm.colourArg1;
 //       }
 //       else if (_bm.blendType == LBT_ALPHA)
 //       {
 //           tss = D3DTSS_ALPHAARG1;
	//		manualD3D = D3DRGBA(mManualBlendColours[_stage][0].r, 
	//			mManualBlendColours[_stage][0].g, 
	//			mManualBlendColours[_stage][0].b, 
	//			_bm.alphaArg1);
 //       }
 //       LayerBlendSource bs = _bm.source1;
 //       for (int i = 0; i < 2; ++i)
 //       {
 //           switch (bs)
 //           {
 //           case LBS_CURRENT:
 //               value = D3DTA_CURRENT;
 //               break;
 //           case LBS_TEXTURE:
 //               value = D3DTA_TEXTURE;
 //               break;
 //           case LBS_DIFFUSE:
 //               value = D3DTA_DIFFUSE;
 //               break;
 //           case LBS_SPECULAR:
 //               value = D3DTA_SPECULAR;
 //               break;
 //           case LBS_MANUAL:
 //               value = D3DTA_TFACTOR;
 //               // Set factor in render state
 //               hr = __SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR,    manualD3D);
 //               break;
 //           }

 //           // Set source
 //           hr = __SetTextureStageState(_stage, tss, value);

 //           // Source2
 //           bs = _bm.source2;
 //           if (_bm.blendType == LBT_COLOUR)
 //           {
 //               tss = D3DTSS_COLORARG2;
 //               manualD3D = D3DRGBA(_bm.colourArg2.r,_bm.colourArg2.g,_bm.colourArg2.b,1.0);
	//			mManualBlendColours[_stage][1] = _bm.colourArg2;
 //           }
 //           else if (_bm.blendType == LBT_ALPHA)
 //           {
 //               tss = D3DTSS_ALPHAARG2;
	//			manualD3D = D3DRGBA(mManualBlendColours[_stage][1].r, 
	//				mManualBlendColours[_stage][1].g, 
	//				mManualBlendColours[_stage][1].b
	//				,_bm.alphaArg2);
 //           }
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setTextureAddressingMode( size_t _stage, TextureUnitState::TextureAddressingMode _tam )
 //   {
 //       HRESULT hr;
 //       D3DTEXTUREADDRESS d3dType;

 //       switch(_tam)
 //       {
 //       case TextureUnitState::TAM_WRAP:
 //           d3dType = D3DTADDRESS_WRAP;
 //           break;
 //       case TextureUnitState::TAM_MIRROR:
 //           d3dType = D3DTADDRESS_MIRROR;
 //           break;
 //       case TextureUnitState::TAM_CLAMP:
 //           d3dType = D3DTADDRESS_CLAMP;
 //           break;
 //       }


 //       hr = __SetTextureStageState(_stage, D3DTSS_ADDRESS, d3dType);
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setSceneBlending( EBlendFactor _sourceFactor, EBlendFactor destFactor )
 //   {
 //       HRESULT hr;
 //       D3DBLEND d3dSrcBlend, d3dDestBlend;

 //       D3DBLEND* pBlend = &d3dSrcBlend;
 //       SceneBlendFactor ogreBlend = _sourceFactor;

 //       for (int i = 0 ; i < 2; ++i)
 //       {
 //           switch(ogreBlend)
 //           {
 //           case SBF_ONE:
 //               *pBlend = D3DBLEND_ONE;
 //               break;
 //           case SBF_ZERO:
 //               *pBlend = D3DBLEND_ZERO;
 //               break;
 //           case SBF_DEST_COLOUR:
 //               *pBlend = D3DBLEND_DESTCOLOR;
 //               break;
 //           case SBF_SOURCE_COLOUR:
 //               *pBlend = D3DBLEND_SRCCOLOR;
 //               break;
 //           case SBF_ONE_MINUS_DEST_COLOUR:
 //               *pBlend = D3DBLEND_INVDESTCOLOR;
 //               break;
 //           case SBF_ONE_MINUS_SOURCE_COLOUR:
 //               *pBlend = D3DBLEND_INVSRCCOLOR;
 //               break;
 //           case SBF_DEST_ALPHA:
 //               *pBlend = D3DBLEND_DESTALPHA;
 //               break;
 //           case SBF_SOURCE_ALPHA:
 //               *pBlend = D3DBLEND_SRCALPHA;
 //               break;
 //           case SBF_ONE_MINUS_DEST_ALPHA:
 //               *pBlend = D3DBLEND_INVDESTALPHA;
 //               break;
 //           case SBF_ONE_MINUS_SOURCE_ALPHA:
 //               *pBlend = D3DBLEND_INVSRCALPHA;
 //               break;
 //           }
 //           ogreBlend = destFactor;
 //           pBlend = &d3dDestBlend;

 //       }

 //       hr = __SetRenderState(D3DRENDERSTATE_SRCBLEND, d3dSrcBlend);

 //       hr = __SetRenderState(D3DRENDERSTATE_DESTBLEND, d3dDestBlend);

 //       // Save last scene blend, because colour write off is simulated 
 //       // through scene blend
 //       m_savedDestFactor = destFactor;
 //       m_savedSrcFactor = _sourceFactor;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setAlphaRejectSettings( CompareFunction _func, unsigned char _value )
 //   {
 //       HRESULT hr;
 //       if (_func != CMPF_ALWAYS_PASS)
 //       {
 //           assert( SUCCEEDED( hr = __SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,  TRUE ) ) &&
	//			"D3DRenderSystem::_setAlphaRejectSettings -> Failed to enable alpha testing" );
 //       }
 //       else
 //       {
 //           assert( SUCCEEDED( hr = __SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,  FALSE ) ) &&
 //               "D3DRenderSystem::_setAlphaRejectSettings -> Failed to disable alpha testing" );
 //       }

 //       // Set always just be sure
 //       hr = __SetRenderState(D3DRENDERSTATE_ALPHAFUNC, convertCompareFunction(_func));

 //       hr = __SetRenderState(D3DRENDERSTATE_ALPHAREF, _value);    
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setViewport(Viewport *vp)
 //   {
	//	if (m_deviceLost)
	//		return;

 //       // Check if viewport is different
 //       if (vp != mActiveViewport || vp->_isUpdated())
 //       {
 //           mActiveViewport = vp;
 //           m_activeRenderTarget = vp->getTarget();
 //           // Ok, it's different. Time to set render target (maybe)
 //           //  and viewport params.
 //           D3DVIEWPORT7 d3dvp;
 //           HRESULT hr;

 //           // Set render target
 //           // TODO - maybe only set when required?
 //           RenderTarget* target;
 //           target = vp->getTarget();
 //           // Get DD Back buffer
 //           LPDIRECTDRAWSURFACE7 pBack;
 //           target->getCustomAttribute("DDBACKBUFFER", &pBack);
	//		if (pBack->IsLost())
	//		{
	//			_notifyDeviceLost();
	//			return;
	//		}

 //           hr = mlpD3DDevice->SetRenderTarget( pBack, 0 );

 //           _setCullingMode( mCullingMode );

 //           // Set viewport dimensions
 //           d3dvp.dwX = vp->getActualLeft();
 //           d3dvp.dwY = vp->getActualTop();
 //           d3dvp.dwWidth = vp->getActualWidth();
 //           d3dvp.dwHeight = vp->getActualHeight();

 //           // Z-values from 0.0 to 1.0 (TODO - standardise with OpenGL?)
 //           d3dvp.dvMinZ = 0.0f;
 //           d3dvp.dvMaxZ = 1.0f;

 //           hr = mlpD3DDevice->SetViewport(&d3dvp);

 //           if (FAILED(hr))
 //               OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting D3D viewport.",
 //               "D3DRenderSystem::_setViewport");

 //           vp->_clearUpdatedFlag();

 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_beginFrame()
 //   {
 //       HRESULT hr;

	//	// Device lost from somewhere outside?
	//	if ( m_deviceLost )
	//	{
	//		_restoreLostDevice();
	//		if ( m_deviceLost )
	//		{
	//			return; // try later
	//		}
	//	}

 //       assert( m_activeViewport && "D3DRenderSystem::_beginFrame -> Cannot begin frame - no viewport selected." );

 //       // Clear the viewport if required
 //       if ( m_activeViewport->getClearEveryFrame() )
 //       {
 //           clearFrameBuffer( FBT_COLOUR | FBT_DEPTH, 
 //               m_activeViewport->getBackgroundColour() );
 //       }

 //       hr = mlpD3DDevice->BeginScene();
 //       if (FAILED(hr))
	//	{
	//		if (hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
	//		{
	//			_notifyDeviceLost();
	//			_restoreLostDevice();
	//			if (m_deviceLost)
	//				return; // try later

	//			if (FAILED(hr = mlpD3DDevice->BeginScene()))
	//				assert( 0 && "Error beginning frame after device restore: "+ getErrorDescription(hr)+
	//				"D3DRenderSystem::_beginFrame");
	//		}
	//		else
	//		{
	//			assert( 0 && "Error beginning frame: " + getErrorDescription(hr)+ "D3DRenderSystem::_beginFrame" );
	//		}
	//	}

 //       // Moved here from _render, no point checking every rendering call
 //       static bool firstTime = true;
 //       if( firstTime )
 //       {
 //           // First-time setup
 //           // Set up some defaults

 //           // Allow alpha blending
 //           hr = __SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_beginFrame -> Error enabling alpha blending option." );

 //           // Allow specular effects
 //           hr = __SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_beginFrame -> Error enabling specular option." );

 //           firstTime = false;
 //       }

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_render( const RenderOperation& _op )
 //   {
 //       HRESULT hr;

	//	if ( m_deviceLost )
	//	{
	//		return;
	//	}

 //       // Exit immediately if there is nothing to render
	//	// Passing 0 arguments causes problems for Direct3D
 //       if ( _op.vertexData->vertexCount == 0 ||
	//		( _op.useIndexes && _op.indexData->indexCount == 0 ) )
	//	{
 //           return;
	//	}

 //       // call superclass
 //       //RenderSystem::_render( _op );
 //       // Set up vertex flags
 //       DWORD d3dVertexFormat = 0;
 //       unsigned int numTexCoords = 0;

 //       // Assume no more than 10 buffers!
 //       static unsigned char* pBufPtrs[10];

 //       // Lock all the buffers first
 //       // They're system memory buffers anyway
 //       const VertexBufferBinding::VertexBufferBindingMap binds = 
 //           _op.vertexData->vertexBufferBinding->getBindings();
 //       VertexBufferBinding::VertexBufferBindingMap::const_iterator bindi, bindend;
 //       bindend = binds.end();
 //       for ( bindi = binds.begin(); bindi != bindend; ++bindi )
 //       {
 //           // lock from vertex start only
 //           pBufPtrs[bindi->first] = static_cast<unsigned char*>(
 //               bindi->second->lock(
 //                   _op.vertexData->vertexStart * bindi->second->getVertexSize(),
 //                   _op.vertexData->vertexCount * bindi->second->getVertexSize(),
 //                   HBL_READ_ONLY )
 //                   );

 //       }

 //       // Determine vertex format

 //       // Struct for data pointers
 //       D3DDRAWPRIMITIVESTRIDEDDATA strideData;

 //       // Iterate over elements
 //       VertexDeclaration::VertexElementList::const_iterator elemi, elemend;
 //       const VertexDeclaration::VertexElementList elems = 
 //           _op.vertexData->vertexDeclaration->getElements();
 //       elemend = elems.end();
 //       for (elemi = elems.begin(); elemi != elemend; ++elemi)
 //       {
 //           // Get a few basic details
 //           const VertexElement& elem = *elemi;
 //           unsigned short source = elem.getSource();
 //           size_t vertexSize = _op.vertexData->vertexDeclaration->getVertexSize( source );
 //           size_t offset = elem.getOffset();
 //           // semantic-specific stuff
 //           switch (elem.getSemantic())
 //           {
 //           case VES_POSITION:
 //               d3dVertexFormat |= D3DFVF_XYZ; // Untransformed  
 //               strideData.position.lpvData = pBufPtrs[source] + offset;
 //               strideData.position.dwStride = static_cast<DWORD>(vertexSize);
 //               // Set up pointer
 //               break;
 //           case VES_NORMAL:
 //               d3dVertexFormat |= D3DFVF_NORMAL; 
 //               strideData.normal.lpvData = pBufPtrs[source] + offset;
 //               strideData.normal.dwStride = static_cast<DWORD>(vertexSize);
 //               break;
 //           case VES_DIFFUSE:
 //               d3dVertexFormat |= D3DFVF_DIFFUSE; 
 //               strideData.diffuse.lpvData = pBufPtrs[source] + offset;
 //               strideData.diffuse.dwStride = static_cast<DWORD>(vertexSize);
 //               break;
 //           case VES_SPECULAR:
 //               d3dVertexFormat |= D3DFVF_SPECULAR; 
 //               strideData.specular.lpvData = pBufPtrs[source] + offset;
 //               strideData.specular.dwStride = static_cast<DWORD>(vertexSize);
 //               break;
 //           case VES_TEXTURE_COORDINATES:
 //               // texcoords must go in order
 //               if (elem.getIndex() != numTexCoords)
 //               {
 //                   OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid vertex format, texture coordinates"
 //                       " must be in order wih no gaps.", "D3DRenderSystem::_render");
 //               }
 //               // Don't add D3DFVF_TEXn flag here, wait until we know how many total
 //               // However, specify size
 //               switch (elem.getType())
 //               {
 //               case VET_FLOAT1:
 //                   d3dVertexFormat |= D3DFVF_TEXCOORDSIZE1(numTexCoords);
 //                   break;
 //               case VET_FLOAT2:
 //                   d3dVertexFormat |= D3DFVF_TEXCOORDSIZE2(numTexCoords);
 //                   break;
 //               case VET_FLOAT3:
 //                   d3dVertexFormat |= D3DFVF_TEXCOORDSIZE3(numTexCoords);
 //                   break;
 //               case VET_FLOAT4:
 //                   d3dVertexFormat |= D3DFVF_TEXCOORDSIZE4(numTexCoords);
 //                   break;
 //               }

 //               strideData.textureCoords[numTexCoords].lpvData = pBufPtrs[source] + offset;
 //               strideData.textureCoords[numTexCoords].dwStride = static_cast<DWORD>(vertexSize);

 //               // Increment number of coords
 //               ++numTexCoords;
 //           }

 //       }
 //       // Add combined texture flag
 //       switch(numTexCoords)
 //       {
 //           case 0: 
 //               // do nothing
 //               break;
 //           case 1:
 //               d3dVertexFormat |= D3DFVF_TEX1;
 //               break;
 //           case 2:
 //               d3dVertexFormat |= D3DFVF_TEX2;
 //               break;
 //           case 3:
 //               d3dVertexFormat |= D3DFVF_TEX3;
 //               break;
 //           case 4:
 //               d3dVertexFormat |= D3DFVF_TEX4;
 //               break;
 //           case 5:
 //               d3dVertexFormat |= D3DFVF_TEX5;
 //               break;
 //           case 6:
 //               d3dVertexFormat |= D3DFVF_TEX6;
 //               break;
 //           case 7:
 //               d3dVertexFormat |= D3DFVF_TEX7;
 //               break;
 //           case 8:
 //               d3dVertexFormat |= D3DFVF_TEX8;
 //               break;
 //       }

 //       // Determine rendering operation
 //       D3DPRIMITIVETYPE primType;
 //       switch (_op.operationType)
 //       {
 //       case RenderOperation::OT_POINT_LIST:
 //           primType = D3DPT_POINTLIST;
 //           break;
 //       case RenderOperation::OT_LINE_LIST:
 //           primType = D3DPT_LINELIST;
 //           break;
 //       case RenderOperation::OT_LINE_STRIP:
 //           primType = D3DPT_LINESTRIP;
 //           break;
 //       case RenderOperation::OT_TRIANGLE_LIST:
 //           primType = D3DPT_TRIANGLELIST;
 //           break;
 //       case RenderOperation::OT_TRIANGLE_STRIP:
 //           primType = D3DPT_TRIANGLESTRIP;
 //           break;
 //       case RenderOperation::OT_TRIANGLE_FAN:
 //           primType = D3DPT_TRIANGLEFAN;
 //           break;
 //       }


 //       if ( _op.useIndexes )
 //       {
 //           // Get pointer to index buffer
 //           // D3D7 only allows 16-bit indexes, this is enforced in buffer manager
 //           unsigned short* pIdx = static_cast<unsigned short*>(
 //               _op.indexData->indexBuffer->lock(
 //                   op.indexData->indexStart,
 //                   op.indexData->indexCount * sizeof(unsigned short),
 //                   HBL_READ_ONLY) );

 //           hr = mlpD3DDevice->DrawIndexedPrimitiveStrided(primType,
 //               d3dVertexFormat, 
 //               &strideData, 
 //               static_cast<DWORD>( _op.vertexData->vertexCount ),
 //               pIdx, 
 //               static_cast<DWORD>( _op.indexData->indexCount )
 //               , 0);
 //       }
 //       else
 //       {
 //           hr = mlpD3DDevice->DrawPrimitiveStrided(primType,
 //               d3dVertexFormat, 
 //               &strideData, 
 //               static_cast<DWORD>( _op.vertexData->vertexCount ),
 //               0);
 //       }

 //       // unlock buffers
 //       for (bindi = binds.begin(); bindi != bindend; ++bindi)
 //       {
 //           bindi->second->unlock();
 //       }

	//	// Ignore errors - lost device issues can occur late in RTTs

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_endFrame(void)
 //   {
	//	if (m_deviceLost)
	//	{
	//		// Don't allow until next begin that restores
	//		return;
	//	}

 //       HRESULT hr;
 //       hr = mlpD3DDevice->EndScene();

 //       if ( FAILED(hr) )
	//	{
	//		if (hr == DDERR_SURFACELOST)
	//		{
	//			// Device lost
	//			_notifyDeviceLost();
	//		}
	//		else
	//		{
	//			// ignore other errors, may not find out about device lost
	//			// early enough in RTT situation
	//		}
	//	}
 //   }
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_notifyDeviceLost(void)
	//{
	//	LogManager::getSingleton().logMessage("!!! Direct3D Device Lost!");
	//	m_deviceLost = true;

	//	fireEvent("DeviceLost");
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_restoreLostDevice(void)
	//{
	//	HRESULT hr = m_activeDDDriver->directDraw()->TestCooperativeLevel();
	//	if (SUCCEEDED(hr))
	//	{
	//		_restoreSurfaces();
	//	}
	//	else if (hr == DDERR_WRONGMODE)
	//	{
	//		 // display mode change
	//		_recreateContext();
	//	}
	//	else if (hr == DDERR_EXCLUSIVEMODEALREADYSET)
	//	{
	//		// This means that some app took exclusive mode access
	//		// we need to sit in a loop till we get back to the right mode.
	//		do
	//		{
	//			Sleep(1000);
	//		}
	//		while (DDERR_EXCLUSIVEMODEALREADYSET == (hr = m_activeDDDriver->directDraw()->TestCooperativeLevel()));
	//		if (SUCCEEDED(hr))
	//		{
	//			// This means that the exclusive mode app relinquished its 
	//			// control and we are back to the safe mode, so simply restore.
	//			_restoreSurfaces();
	//		}
	//		else if (hr == DDERR_WRONGMODE)
	//		{
	//			// This means that the exclusive mode app relinquished its 
	//			// control BUT we are back to some strange mode, so destroy
	//			// and recreate.
	//			_recreateContext();
	//		}
	//		else
	//		{
	//			OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
	//				getErrorDescription(hr),
	//				"D3DRenderSystem::_restoreLostDevice");
	//		}
	//	}
	//	else if (hr == DDERR_NOEXCLUSIVEMODE)
	//	{
	//		// Not sure what to do here, it tends to fix itself after a few tries.
	//		return;
	//	}
	//	else
	//	{
	//		// Some other error has occurred which we haven't handled.
	//		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
	//			getErrorDescription(hr),
	//			"D3DRenderSystem::_restoreLostDevice");
	//	}

	//	LogManager::getSingleton().logMessage("!!! Direct3D Device successfully restored.");
	//	m_deviceLost = false;
	//	fireEvent("DeviceRestored");
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_restoreSurfaces()
	//{
	//	HRESULT hr = m_activeDDDriver->directDraw()->RestoreAllSurfaces();
	//	assert( SUCCEEDED(hr) && getErrorDescription(hr) + "D3DRenderSystem::_restoreLostDevice" );

	//	// Need to reload all textures since surfaces are now blank
	//	static_cast<D3DTextureManager*>(m_textureManager)
	//		->reloadAfterLostDevice();

	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_recreateContext()
	//{
	//	// restore primary render window if fullscreen
	//	for (RenderTargetMap::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i)
	//	{
	//		RenderTarget* rt = i->second;
	//		if (rt->isPrimary())
	//		{
	//			D3D7RenderWindow* d3dwin = static_cast<D3D7RenderWindow*>(rt);
	//			d3dwin->releaseDDSurfaces();
	//			d3dwin->createDDSurfaces();
	//			d3dwin->createDepthBuffer();
	//			break;
	//		}
	//	}
	//}
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setCullingMode(CullingMode mode)
 //   {
 //       HRESULT hr;
 //       DWORD d3dMode;

 //       mCullingMode = mode;

 //       if (mode == CULL_NONE)
 //       {
 //           d3dMode = D3DCULL_NONE;
 //       }
 //       else if( mode == CULL_CLOCKWISE )
 //       {
 //           if( (m_activeRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
 //               (!m_activeRenderTarget->requiresTextureFlipping() && mInvertVertexWinding))
 //               d3dMode = D3DCULL_CCW;
 //           else
 //               d3dMode = D3DCULL_CW;
 //       }
 //       else if (mode == CULL_ANTICLOCKWISE)
 //       {
 //           if( (m_activeRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
 //               (!m_activeRenderTarget->requiresTextureFlipping() && mInvertVertexWinding))
 //               d3dMode = D3DCULL_CW;
 //           else
 //               d3dMode = D3DCULL_CCW;
 //       }

 //       hr = __SetRenderState(D3DRENDERSTATE_CULLMODE, d3dMode);
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setCullingMode -> Unable to set D3D culling mode." );

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setDepthBufferParams( bool _depthTest, bool _depthWrite, CompareFunction _depthFunction )
 //   {
 //       _setDepthBufferCheckEnabled( _depthTest );
 //       _setDepthBufferWriteEnabled( _depthWrite );
 //       _setDepthBufferFunction( _depthFunction );

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setDepthBufferCheckEnabled( bool _enabled )
 //   {
 //       HRESULT hr;

 //       if ( _enabled )
 //       {
 //           // Use w-buffer if available and enabled
 //           if (mWBuffer && m_D3DDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER)
 //               hr = __SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_USEW);
 //           else
 //               hr = __SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_setDepthBufferCheckEnabled -> Error setting depth buffer test state." );
 //       }
 //       else
 //       {
 //           hr = __SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
 //           assert( SUCCEEDED(hr) && "D3DRenderSystem::_setDepthBufferCheckEnabled -> Error setting depth buffer test state." );
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setDepthBufferWriteEnabled( bool _enabled )
 //   {
 //       HRESULT hr;

 //       hr = __SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, _enabled );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setDepthBufferWriteEnabled -> Error setting depth buffer write state." );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setDepthBufferFunction( CompareFunction _func )
 //   {
 //       HRESULT hr = __SetRenderState(D3DRENDERSTATE_ZFUNC, 
 //           convertCompareFunction(_func) );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setDepthBufferFunction -> Error setting depth buffer test function." );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setDepthBias( unsigned short _bias )
 //   {
 //       HRESULT hr = __SetRenderState(D3DRENDERSTATE_ZBIAS, _bias );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::_setDepthBias -> Error setting depth bias." );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   String D3DRenderSystem::getErrorDescription( long _errCode ) const
 //   {
 //       char* errDesc;
 //       DWORD i;

 //       errDesc = new char[255];
 //       // Try windows errors first
 //       i = FormatMessage(
 //           FORMAT_MESSAGE_FROM_HMODULE |
 //           FORMAT_MESSAGE_FROM_SYSTEM |
 //           FORMAT_MESSAGE_IGNORE_INSERTS,
 //           NULL,
 //           DDERR_SURFACELOST,
 //           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
 //           (LPTSTR) errDesc,
 //           255,
 //           NULL
 //           );


 //       if( i == 0 )
 //       {
 //           // Not found in windows message descriptions
 //           switch( _errCode )
 //           {
 //           case DD_OK:
 //               strcpy(errDesc, "DD_OK: The request completed successfully.");
 //               break;
 //           case DDERR_ALREADYINITIALIZED:
 //               strcpy(errDesc, "DDERR_ALREADYINITIALIZED: The object has already been initialized.");
 //               break;
 //           case DDERR_BLTFASTCANTCLIP :
 //               strcpy(errDesc, "DDERR_BLTFASTCANTCLIP: A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method.");
 //               break;
 //           case DDERR_CANNOTATTACHSURFACE:
 //               strcpy(errDesc, "DDERR_CANNOTATTACHSURFACE: A surface cannot be attached to another requested surface.");
 //               break;
 //           case DDERR_CANNOTDETACHSURFACE:
 //               strcpy(errDesc, "DDERR_CANNOTATTACHSURFACE: A surface cannot be detached from another requested surface.");
 //               break;
 //           case DDERR_CANTCREATEDC:
 //               strcpy(errDesc, "DDERR_CANTCREATEDC: Windows can not create any more device contexts (DCs), or a DC was requested for a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed.");
 //               break;
 //           case DDERR_CANTDUPLICATE:
 //               strcpy(errDesc, "DDERR_CANTDUPLICATE: Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.");
 //               break;
 //           case DDERR_CANTLOCKSURFACE:
 //               strcpy(errDesc, "DDERR_CANTLOCKSURFACE: Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.");
 //               break;
 //           case DDERR_CANTPAGELOCK:
 //               strcpy(errDesc, "DDERR_CANTPAGELOCK: An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface.");
 //               break;
 //           case DDERR_CANTPAGEUNLOCK:
 //               strcpy(errDesc, "DDERR_CANTPAGEUNLOCK: An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface.");
 //               break;
 //           case DDERR_CLIPPERISUSINGHWND:
 //               strcpy(errDesc, "DDERR_CLIPPERISUSINGHWND: An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.");
 //               break;
 //           case DDERR_COLORKEYNOTSET:
 //               strcpy(errDesc, "DDERR_COLORKEYNOTSET: No source color key is specified for this operation.");
 //               break;
 //           case DDERR_CURRENTLYNOTAVAIL:
 //               strcpy(errDesc, "DDERR_CURRENTLYNOTAVAIL: No support is currently available.");
 //               break;
 //           case DDERR_DCALREADYCREATED:
 //               strcpy(errDesc, "DDERR_DCALREADYCREATED: A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.");
 //               break;
 //           case DDERR_DEVICEDOESNTOWNSURFACE:
 //               strcpy(errDesc, "DDERR_DEVICEDOESNTOWNSURFACE: Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.");
 //               break;
 //           case DDERR_DIRECTDRAWALREADYCREATED:
 //               strcpy(errDesc, "DDERR_DIRECTDRAWALREADYCREATED: A DirectDraw object representing this driver has already been created for this process.");
 //               break;
 //           case DDERR_EXCEPTION:
 //               strcpy(errDesc, "DDERR_EXCEPTION: An exception was encountered while performing the requested operation.");
 //               break;
 //           case DDERR_EXCLUSIVEMODEALREADYSET:
 //               strcpy(errDesc, "DDERR_EXCLUSIVEMODEALREADYSET: An attempt was made to set the cooperative level when it was already set to exclusive.");
 //               break;
 //           case DDERR_EXPIRED:
 //               strcpy(errDesc, "DDERR_EXPIRED: The data has expired and is therefore no longer valid.");
 //               break;
 //           case DDERR_GENERIC:
 //               strcpy(errDesc, "DDERR_GENERIC: There is an undefined error condition.");
 //               break;
 //           case DDERR_HEIGHTALIGN:
 //               strcpy(errDesc, "DDERR_HEIGHTALIGN: The height of the provided rectangle is not a multiple of the required alignment.");
 //               break;
 //           case DDERR_HWNDALREADYSET:
 //               strcpy(errDesc, "DDERR_HWNDALREADYSET: The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.");
 //               break;
 //           case DDERR_HWNDSUBCLASSED:
 //               strcpy(errDesc, "DDERR_HWNDSUBCLASSED: DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed.");
 //               break;
 //           case DDERR_IMPLICITLYCREATED:
 //               strcpy(errDesc, "DDERR_IMPLICITLYCREATED: The surface cannot be restored because it is an implicitly created surface.");
 //               break;
 //           case DDERR_INCOMPATIBLEPRIMARY:
 //               strcpy(errDesc, "DDERR_INCOMPATIBLEPRIMARY: The primary surface creation request does not match with the existing primary surface.");
 //               break;
 //           case DDERR_INVALIDCAPS:
 //               strcpy(errDesc, "DDERR_INVALIDCAPS: One or more of the capability bits passed to the callback function are incorrect.");
 //               break;
 //           case DDERR_INVALIDCLIPLIST:
 //               strcpy(errDesc, "DDERR_INVALIDCLIPLIST: DirectDraw does not support the provided clip list.");
 //               break;
 //           case DDERR_INVALIDDIRECTDRAWGUID:
 //               strcpy(errDesc, "DDERR_INVALIDDIRECTDRAWGUID: The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.");
 //               break;
 //           case DDERR_INVALIDMODE:
 //               strcpy(errDesc, "DDERR_INVALIDMODE: DirectDraw does not support the requested mode.");
 //               break;
 //           case DDERR_INVALIDOBJECT:
 //               strcpy(errDesc, "DDERR_INVALIDOBJECT: DirectDraw received a pointer that was an invalid DirectDraw object.");
 //               break;
 //           case DDERR_INVALIDPARAMS:
 //               strcpy(errDesc, "DDERR_INVALIDPARAMS: One or more of the parameters passed to the method are incorrect.");
 //               break;
 //           case DDERR_INVALIDPIXELFORMAT:
 //               strcpy(errDesc, "DDERR_INVALIDPIXELFORMAT: The pixel format was invalid as specified.");
 //               break;
 //           case DDERR_INVALIDPOSITION:
 //               strcpy(errDesc, "DDERR_INVALIDPOSITION: The position of the overlay on the destination is no longer legal.");
 //               break;
 //           case DDERR_INVALIDRECT:
 //               strcpy(errDesc, "DDERR_INVALIDRECT: The provided rectangle was invalid.");
 //               break;
 //           case DDERR_INVALIDSTREAM:
 //               strcpy(errDesc, "DDERR_INVALIDSTREAM: The specified stream contains invalid data.");
 //               break;
 //           case DDERR_INVALIDSURFACETYPE:
 //               strcpy(errDesc, "DDERR_INVALIDSURFACETYPE: The requested operation could not be performed because the surface was of the wrong type.");
 //               break;
 //           case DDERR_LOCKEDSURFACES:
 //               strcpy(errDesc, "DDERR_LOCKEDSURFACES: One or more surfaces are locked, causing the failure of the requested operation.");
 //               break;
 //           case DDERR_MOREDATA:
 //               strcpy(errDesc, "DDERR_MOREDATA: There is more data available than the specified buffer size can hold.");
 //               break;
 //           case DDERR_NO3D:
 //               strcpy(errDesc, "DDERR_NO3D: No 3-D hardware or emulation is present.");
 //               break;
 //           case DDERR_NOALPHAHW:
 //               strcpy(errDesc, "DDERR_NOALPHAHW: No alpha acceleration hardware is present or available, causing the failure of the requested operation.");
 //               break;
 //           case DDERR_NOBLTHW:
 //               strcpy(errDesc, "DDERR_NOBLTHW: No blitter hardware is present.");
 //               break;
 //           case DDERR_NOCLIPLIST:
 //               strcpy(errDesc, "DDERR_NOCLIPLIST: No clip list is available.");
 //               break;
 //           case DDERR_NOCLIPPERATTACHED:
 //               strcpy(errDesc, "DDERR_NOCLIPPERATTACHED: No DirectDrawClipper object is attached to the surface object.");
 //               break;
 //           case DDERR_NOCOLORCONVHW:
 //               strcpy(errDesc, "DDERR_NOCOLORCONVHW: The operation cannot be carried out because no color-conversion hardware is present or available.");
 //               break;
 //           case DDERR_NOCOLORKEY:
 //               strcpy(errDesc, "DDERR_NOCOLORKEY: The surface does not currently have a color key.");
 //               break;
 //           case DDERR_NOCOLORKEYHW:
 //               strcpy(errDesc, "DDERR_NOCOLORKEYHW: The operation cannot be carried out because there is no hardware support for the destination color key.");
 //               break;
 //           case DDERR_NOCOOPERATIVELEVELSET:
 //               strcpy(errDesc, "DDERR_NOCOOPERATIVELEVELSET: A create function is called without the IDirectDraw7::SetCooperativeLevel method being called.");
 //               break;
 //           case DDERR_NODC:
 //               strcpy(errDesc, "DDERR_NODC: No DC has ever been created for this surface.");
 //               break;
 //           case DDERR_NODDROPSHW:
 //               strcpy(errDesc, "DDERR_NODDROPSHW: No DirectDraw raster operation (ROP) hardware is available.");
 //               break;
 //           case DDERR_NODIRECTDRAWHW:
 //               strcpy(errDesc, "DDERR_NODIRECTDRAWHW: Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.");
 //               break;
 //           case DDERR_NODIRECTDRAWSUPPORT:
 //               strcpy(errDesc, "DDERR_NODIRECTDRAWSUPPORT: DirectDraw support is not possible with the current display driver.");
 //               break;
 //           case DDERR_NOEMULATION:
 //               strcpy(errDesc, "DDERR_NOEMULATION: Software emulation is not available.");
 //               break;
 //           case DDERR_NOEXCLUSIVEMODE:
 //               strcpy(errDesc, "DDERR_NOEXCLUSIVEMODE: The operation requires the application to have exclusive mode, but the application does not have exclusive mode.");
 //               break;
 //           case DDERR_NOFLIPHW:
 //               strcpy(errDesc, "DDERR_NOFLIPHW: Flipping visible surfaces is not supported.");
 //               break;
 //           case DDERR_NOFOCUSWINDOW:
 //               strcpy(errDesc, "DDERR_NOFOCUSWINDOW: An attempt was made to create or set a device window without first setting the focus window.");
 //               break;
 //           case DDERR_NOGDI:
 //               strcpy(errDesc, "DDERR_NOGDI: No GDI is present.");
 //               break;
 //           case DDERR_NOHWND:
 //               strcpy(errDesc, "DDERR_NOHWND: Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.");
 //               break;
 //           case DDERR_NOMIPMAPHW:
 //               strcpy(errDesc, "DDERR_NOMIPMAPHW: The operation cannot be carried out because no mipmap capable texture mapping hardware is present or available.");
 //               break;
 //           case DDERR_NOMIRRORHW:
 //               strcpy(errDesc, "DDERR_NOMIRRORHW: The operation cannot be carried out because no mirroring hardware is present or available.");
 //               break;
 //           case DDERR_NONONLOCALVIDMEM:
 //               strcpy(errDesc, "DDERR_NONONLOCALVIDMEM: An attempt was made to allocate non-local video memory from a device that does not support non-local video memory.");
 //               break;
 //           case DDERR_NOOPTIMIZEHW:
 //               strcpy(errDesc, "DDERR_NOOPTIMIZEHW: The device does not support optimized surfaces.");
 //               break;
 //           case DDERR_NOOVERLAYDEST:
 //               strcpy(errDesc, "DDERR_NOOVERLAYDEST: The IDirectDrawSurface4::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface7::UpdateOverlay method has not been called on to establish a destination.");
 //               break;
 //           case DDERR_NOOVERLAYHW:
 //               strcpy(errDesc, "DDERR_NOOVERLAYHW: The operation cannot be carried out because no overlay hardware is present or available.");
 //               break;
 //           case DDERR_NOPALETTEATTACHED:
 //               strcpy(errDesc, "DDERR_NOPALETTEATTACHED: No palette object is attached to this surface.");
 //               break;
 //           case DDERR_NOPALETTEHW:
 //               strcpy(errDesc, "DDERR_NOPALETTEHW: There is no hardware support for 16- or 256-color palettes.");
 //               break;
 //           case DDERR_NORASTEROPHW:
 //               strcpy(errDesc, "DDERR_NORASTEROPHW: The operation cannot be carried out because no appropriate raster operation hardware is present or available.");
 //               break;
 //           case DDERR_NOROTATIONHW:
 //               strcpy(errDesc, "DDERR_NOROTATIONHW: The operation cannot be carried out because no rotation hardware is present or available.");
 //               break;
 //           case DDERR_NOSTRETCHHW:
 //               strcpy(errDesc, "DDERR_NOSTRETCHHW: The operation cannot be carried out because there is no hardware support for stretching.");
 //               break;
 //           case DDERR_NOT4BITCOLOR:
 //               strcpy(errDesc, "DDERR_NOT4BITCOLOR: The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette.");
 //               break;
 //           case DDERR_NOT4BITCOLORINDEX:
 //               strcpy(errDesc, "DDERR_NOT4BITCOLORINDEX: The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette.");
 //               break;
 //           case DDERR_NOT8BITCOLOR:
 //               strcpy(errDesc, "DDERR_NOT8BITCOLOR: The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette.");
 //               break;
 //           case DDERR_NOTAOVERLAYSURFACE:
 //               strcpy(errDesc, "DDERR_NOTAOVERLAYSURFACE: An overlay component is called for a non-overlay surface.");
 //               break;
 //           case DDERR_NOTEXTUREHW:
 //               strcpy(errDesc, "DDERR_NOTEXTUREHW: The operation cannot be carried out because no texture-mapping hardware is present or available.");
 //               break;
 //           case DDERR_NOTFLIPPABLE:
 //               strcpy(errDesc, "DDERR_NOTFLIPPABLE: An attempt has been made to flip a surface that cannot be flipped.");
 //               break;
 //           case DDERR_NOTFOUND:
 //               strcpy(errDesc, "DDERR_NOTFOUND: The requested item was not found.");
 //               break;
 //           case DDERR_NOTINITIALIZED:
 //               strcpy(errDesc, "DDERR_NOTINITIALIZED: An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.");
 //               break;
 //           case DDERR_NOTLOADED:
 //               strcpy(errDesc, "DDERR_NOTLOADED: The surface is an optimized surface, but it has not yet been allocated any memory.");
 //               break;
 //           case DDERR_NOTLOCKED:
 //               strcpy(errDesc, "DDERR_NOTLOCKED: An attempt is made to unlock a surface that was not locked.");
 //               break;
 //           case DDERR_NOTPAGELOCKED:
 //               strcpy(errDesc, "DDERR_NOTPAGELOCKED: An attempt is made to page unlock a surface with no outstanding page locks.");
 //               break;
 //           case DDERR_NOTPALETTIZED:
 //               strcpy(errDesc, "DDERR_NOTPALETTIZED: The surface being used is not a palette-based surface.");
 //               break;
 //           case DDERR_NOVSYNCHW:
 //               strcpy(errDesc, "DDERR_NOVSYNCHW: The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations.");
 //               break;
 //           case DDERR_NOZBUFFERHW:
 //               strcpy(errDesc, "DDERR_NOZBUFFERHW: The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers.");
 //               break;
 //           case DDERR_NOZOVERLAYHW:
 //               strcpy(errDesc, "DDERR_NOZOVERLAYHW: The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays.");
 //               break;
 //           case DDERR_OUTOFCAPS:
 //               strcpy(errDesc, "DDERR_OUTOFCAPS: The hardware needed for the requested operation has already been allocated.");
 //               break;
 //           case DDERR_OUTOFMEMORY:
 //               strcpy(errDesc, "DDERR_OUTOFMEMORY: DirectDraw does not have enough memory to perform the operation.");
 //               break;
 //           case DDERR_OUTOFVIDEOMEMORY:
 //               strcpy(errDesc, "DDERR_OUTOFVIDEOMEMORY: DirectDraw does not have enough display memory to perform the operation.");
 //               break;
 //           case DDERR_OVERLAPPINGRECTS:
 //               strcpy(errDesc, "DDERR_OVERLAPPINGRECTS: Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other.");
 //               break;
 //           case DDERR_OVERLAYCANTCLIP:
 //               strcpy(errDesc, "DDERR_OVERLAYCANTCLIP: The hardware does not support clipped overlays.");
 //               break;
 //           case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
 //               strcpy(errDesc, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE: An attempt was made to have more than one color key active on an overlay.");
 //               break;
 //           case DDERR_OVERLAYNOTVISIBLE:
 //               strcpy(errDesc, "DDERR_OVERLAYNOTVISIBLE: The IDirectDrawSurface7::GetOverlayPosition method is called on a hidden overlay.");
 //               break;
 //           case DDERR_PALETTEBUSY:
 //               strcpy(errDesc, "DDERR_PALETTEBUSY: Access to this palette is refused because the palette is locked by another thread.");
 //               break;
 //           case DDERR_PRIMARYSURFACEALREADYEXISTS:
 //               strcpy(errDesc, "DDERR_PRIMARYSURFACEALREADYEXISTS: This process has already created a primary surface.");
 //               break;
 //           case DDERR_REGIONTOOSMALL:
 //               strcpy(errDesc, "DDERR_REGIONTOOSMALL: The region passed to the IDirectDrawClipper::GetClipList method is too small.");
 //               break;
 //           case DDERR_SURFACEALREADYATTACHED:
 //               strcpy(errDesc, "DDERR_SURFACEALREADYATTACHED: An attempt was made to attach a surface to another surface to which it is already attached.");
 //               break;
 //           case DDERR_SURFACEALREADYDEPENDENT:
 //               strcpy(errDesc, "DDERR_SURFACEALREADYDEPENDENT: An attempt was made to make a surface a dependency of another surface to which it is already dependent.");
 //               break;
 //           case DDERR_SURFACEBUSY:
 //               strcpy(errDesc, "DDERR_SURFACEBUSY: Access to the surface is refused because the surface is locked by another thread.");
 //               break;
 //           case DDERR_SURFACEISOBSCURED:
 //               strcpy(errDesc, "DDERR_SURFACEISOBSCURED: Access to the surface is refused because the surface is obscured.");
 //               break;
 //           case DDERR_SURFACELOST:
 //               strcpy(errDesc, "DDERR_SURFACELOST: Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it.");
 //               break;
 //           case DDERR_SURFACENOTATTACHED:
 //               strcpy(errDesc, "DDERR_SURFACENOTATTACHED: The requested surface is not attached.");
 //               break;
 //           case DDERR_TOOBIGHEIGHT:
 //               strcpy(errDesc, "DDERR_TOOBIGHEIGHT: The height requested by DirectDraw is too large.");
 //               break;
 //           case DDERR_TOOBIGSIZE:
 //               strcpy(errDesc, "DDERR_TOOBIGSIZE: The size requested by DirectDraw is too large. However, the individual height and width are valid sizes.");
 //               break;
 //           case DDERR_TOOBIGWIDTH:
 //               strcpy(errDesc, "DDERR_TOOBIGWIDTH: The width requested by DirectDraw is too large.");
 //               break;
 //           case DDERR_UNSUPPORTED:
 //               strcpy(errDesc, "DDERR_UNSUPPORTED: The operation is not supported.");
 //               break;
 //           case DDERR_UNSUPPORTEDFORMAT:
 //               strcpy(errDesc, "DDERR_UNSUPPORTEDFORMAT: The FourCC format requested is not supported by DirectDraw.");
 //               break;
 //           case DDERR_UNSUPPORTEDMASK:
 //               strcpy(errDesc, "DDERR_UNSUPPORTEDMASK: The bitmask in the pixel format requested is not supported by DirectDraw.");
 //               break;
 //           case DDERR_UNSUPPORTEDMODE:
 //               strcpy(errDesc, "DDERR_UNSUPPORTEDMODE: The display is currently in an unsupported mode.");
 //               break;
 //           case DDERR_VERTICALBLANKINPROGRESS:
 //               strcpy(errDesc, "DDERR_VERTICALBLANKINPROGRESS: A vertical blank is in progress.");
 //               break;
 //           case DDERR_VIDEONOTACTIVE:
 //               strcpy(errDesc, "DDERR_VIDEONOTACTIVE: The video port is not active.");
 //               break;
 //           case DDERR_WASSTILLDRAWING:
 //               strcpy(errDesc, "DDERR_WASSTILLDRAWING: The previous blit operation that is transferring information to or from this surface is incomplete.");
 //               break;
 //           case DDERR_WRONGMODE:
 //               strcpy(errDesc, "DDERR_WRONGMODE: This surface cannot be restored because it was created in a different mode.");
 //               break;
 //           case DDERR_XALIGN:
 //               strcpy(errDesc, "DDERR_XALIGN: The provided rectangle was not horizontally aligned on a required boundary.");
 //               break;
 //           default:
 //               strcpy(errDesc, "Unknown error code.");
 //           }
 //       }
 //       return errDesc;
 //   }

 //   /*
 //   D3D_OK
 //   D3DERR_BADMAJORVERSION
 //   D3DERR_BADMINORVERSION
 //   D3DERR_COLORKEYATTACHED
 //   D3DERR_CONFLICTINGTEXTUREFILTER
 //   D3DERR_CONFLICTINGTEXTUREPALETTE
 //   D3DERR_CONFLICTINGRENDERSTATE
 //   D3DERR_DEVICEAGGREGATED (new for DirectX 5.0)
 //   D3DERR_EXECUTE_CLIPPED_FAILED
 //   D3DERR_EXECUTE_CREATE_FAILED
 //   D3DERR_EXECUTE_DESTROY_FAILED
 //   D3DERR_EXECUTE_FAILED
 //   D3DERR_EXECUTE_LOCK_FAILED
 //   D3DERR_EXECUTE_LOCKED
 //   D3DERR_EXECUTE_NOT_LOCKED
 //   D3DERR_EXECUTE_UNLOCK_FAILED
 //   D3DERR_INITFAILED (new for DirectX 5.0)
 //   D3DERR_INBEGIN (new for DirectX 5.0)
 //   D3DERR_INVALID_DEVICE (new for DirectX 5.0)
 //   D3DERR_INVALIDCURRENTVIEWPORT (new for DirectX 5.0)
 //   D3DERR_INVALIDMATRIX
 //   D3DERR_INVALIDPALETTE(new for DirectX 5.0)
 //   D3DERR_INVALIDPRIMITIVETYPE (new for DirectX 5.0)
 //   D3DERR_INVALIDRAMPTEXTURE (new for DirectX 5.0)
 //   D3DERR_INVALIDVERTEXFORMAT (new for DirectX 6.0)
 //   D3DERR_INVALIDVERTEXTYPE (new for DirectX 5.0)
 //   D3DERR_LIGHT_SET_FAILED
 //   D3DERR_LIGHTHASVIEWPORT (new for DirectX 5.0)
 //   D3DERR_LIGHTNOTINTHISVIEWPORT (new for DirectX 5.0)
 //   D3DERR_MATERIAL_CREATE_FAILED
 //   D3DERR_MATERIAL_DESTROY_FAILED
 //   D3DERR_MATERIAL_GETDATA_FAILED
 //   D3DERR_MATERIAL_SETDATA_FAILED
 //   D3DERR_MATRIX_CREATE_FAILED
 //   D3DERR_MATRIX_DESTROY_FAILED
 //   D3DERR_MATRIX_GETDATA_FAILED
 //   D3DERR_MATRIX_SETDATA_FAILED
 //   D3DERR_NOCURRENTVIEWPORT (new for DirectX 5.0)
 //   D3DERR_NOTINBEGIN (new for DirectX 5.0)
 //   D3DERR_NOVIEWPORTS (new for DirectX 5.0)
 //   D3DERR_SCENE_BEGIN_FAILED
 //   D3DERR_SCENE_END_FAILED
 //   D3DERR_SCENE_IN_SCENE
 //   D3DERR_SCENE_NOT_IN_SCENE
 //   D3DERR_SETVIEWPORTDATA_FAILED
 //   D3DERR_STENCILBUFFER_NOTPRESENT
 //   D3DERR_SURFACENOTINVIDMEM (new for DirectX 5.0)
 //   D3DERR_TEXTURE_BADSIZE (new for DirectX 5.0)
 //   D3DERR_TEXTURE_CREATE_FAILED
 //   D3DERR_TEXTURE_DESTROY_FAILED
 //   D3DERR_TEXTURE_GETSURF_FAILED
 //   D3DERR_TEXTURE_LOAD_FAILED
 //   D3DERR_TEXTURE_LOCK_FAILED
 //   D3DERR_TEXTURE_LOCKED
 //   D3DERR_TEXTURE_NO_SUPPORT
 //   D3DERR_TEXTURE_NOT_LOCKED
 //   D3DERR_TEXTURE_SWAP_FAILED
 //   D3DERR_TEXTURE_UNLOCK_FAILED
 //   D3DERR_TOOMANYOPERATIONS
 //   D3DERR_TOOMANYPRIMITIVES
 //   D3DERR_UNSUPPORTEDALPHAARG
 //   D3DERR_UNSUPPORTEDALPHAOPERATION
 //   D3DERR_UNSUPPORTEDCOLORARG
 //   D3DERR_UNSUPPORTEDCOLOROPERATION
 //   D3DERR_UNSUPPORTEDFACTORVALUE
 //   D3DERR_UNSUPPORTEDTEXTUREFILTER
 //   D3DERR_VBUF_CREATE_FAILED
 //   D3DERR_VERTEXBUFFERLOCKED
 //   D3DERR_VERTEXBUFFEROPTIMIZED
 //   D3DERR_VIEWPORTDATANOTSET (new for DirectX 5.0)
 //   D3DERR_VIEWPORTHASNODEVICE (new for DirectX 5.0)
 //   D3DERR_WRONGTEXTUREFORMAT
 //   D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY (new for DirectX 5.0)
 //   D3DERR_ZBUFF_NEEDS_VIDEOMEMORY (new for DirectX 5.0)
 //   D3DERR_ZBUFFER_NOTPRESENT
 //   */

    //////////////////////////////////////////////////////////////////////////
    DDDriverList* D3DRenderSystem::getDirectDrawDrivers()
    {
        if( !m_driverList )
        {
            m_driverList = new DDDriverList( m_logInterface );
        }

        return m_driverList;
    }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setLightingEnabled( bool _enabled )
 //   {
 //       // Call D3D
 //       HRESULT hr = __SetRenderState( D3DRENDERSTATE_LIGHTING, _enabled );
 //       assert( SUCCEEDED(hr) && "D3DRenderSystem::setLightingEnabled -> Error lighting status." );
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setFog( FogMode _mode, const float* _colour, float _density, float* _start, float _end )
 //   {
 //       HRESULT hr;

 //       D3DRENDERSTATETYPE fogType, fogTypeNot;

 //       if ( m_D3DDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE )
 //       {
 //           fogType = D3DRENDERSTATE_FOGTABLEMODE;
 //           fogTypeNot = D3DRENDERSTATE_FOGVERTEXMODE;
 //       }
 //       else
 //       {
 //           fogType = D3DRENDERSTATE_FOGVERTEXMODE;
 //           fogTypeNot = D3DRENDERSTATE_FOGTABLEMODE;
 //       }

 //       if( _mode == FOG_NONE )
 //       {
 //           // just disable
 //           hr = __SetRenderState( fogType, D3DFOG_NONE );
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGENABLE, FALSE );
 //       }
 //       else
 //       {
 //           // Allow fog
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGENABLE, TRUE );
 //           hr = __SetRenderState( fogTypeNot, D3DFOG_NONE );
 //           switch( _mode )
 //           {
 //           case FOG_EXP:
 //               hr = __SetRenderState( fogType, D3DFOG_EXP );
 //               break;
 //           case FOG_EXP2:
 //               hr = __SetRenderState( fogType, D3DFOG_EXP2 );
 //               break;
 //           case FOG_LINEAR:
 //               hr = __SetRenderState( fogType, D3DFOG_LINEAR);
 //               break;

 //           }

	//		mt::Color col( _colour[0], _colour[1], _colour[2], _colour[3] );
	//		
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGCOLOR, colour.getAsARGB() );
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGSTART, *((LPDWORD)(&_start)) );
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGEND, *((LPDWORD)(&_end)) );
 //           hr = __SetRenderState( D3DRENDERSTATE_FOGDENSITY, *((LPDWORD)(&_density)) );
 //       }


 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::convertColourValue( const ColourValue& colour, uint32* pDest )
 //   {
 //       *pDest = colour.getAsARGB();
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_makeProjectionMatrix( const float _fovy, float _aspect, 
 //       float _nearPlane, float _farPlane, float* _dest, bool _forGpuProgram )
 //   {
 //       float theta(_fovy * 0.5f);
 //       float h = 1 / Math::Tan(theta);
 //       float w = h / aspect;
 //       float q, qn;
 //       if (farPlane == 0)
 //       {
 //           q = 1 - Frustum::INFINITE_FAR_PLANE_ADJUST;
 //           qn = nearPlane * (Frustum::INFINITE_FAR_PLANE_ADJUST - 1);
 //       }
 //       else
 //       {
 //           q = farPlane / ( farPlane - nearPlane );
 //           qn = -q * nearPlane;
 //       }

 //       dest = Matrix4::ZERO;
 //       dest[0][0] = w;
 //       dest[1][1] = h;

 //       if (forGpuProgram)
 //       {
 //           dest[2][2] = -q;
 //           dest[3][2] = -1.0f;
 //       }
 //       else
 //       {
 //           dest[2][2] = q;
 //           dest[3][2] = 1.0f;
 //       }

 //       dest[2][3] = qn;

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_makeOrthoMatrix(const Radian& fovy, Real aspect, Real nearPlane, Real farPlane, 
 //       Matrix4& dest, bool forGpuProgram )
 //   {
 //       Radian thetaY (fovy / 2.0f);
 //       Real tanThetaY = Math::Tan(thetaY);

 //       //Real thetaX = thetaY * aspect;
 //       Real tanThetaX = tanThetaY * aspect; //Math::Tan(thetaX);
 //       Real half_w = tanThetaX * nearPlane;
 //       Real half_h = tanThetaY * nearPlane;
 //       Real iw = 1.0 / half_w;
 //       Real ih = 1.0 / half_h;
 //       Real q;
 //       if (farPlane == 0)
 //       {
 //           q = 0;
 //       }
 //       else
 //       {
 //           q = 1.0 / (farPlane - nearPlane);
 //       }

 //       dest = Matrix4::ZERO;
 //       dest[0][0] = iw;
 //       dest[1][1] = ih;
 //       dest[2][2] = q;
 //       dest[2][3] = -nearPlane / (farPlane - nearPlane);
 //       dest[3][3] = 1;

 //       if (forGpuProgram)
 //       {
 //           dest[2][2] = -dest[2][2];
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setPolygonMode(PolygonMode level)
 //   {
 //       D3DFILLMODE d3dmode;

 //       switch(level)
 //       {
 //       case PM_POINTS:
 //           d3dmode = D3DFILL_POINT;
 //           break;
 //       case PM_WIREFRAME:
 //           d3dmode = D3DFILL_WIREFRAME;
 //           break;
 //       case PM_SOLID:
 //           d3dmode = D3DFILL_SOLID;
 //           break;

 //       }

 //       HRESULT hr = __SetRenderState(D3DRENDERSTATE_FILLMODE, d3dmode);
 //       if (FAILED(hr))
 //       {
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting rasterisation mode.", 
 //               "D3DRenderSystem::setRasterisationMode");
 //       }


 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setStencilCheckEnabled(bool enabled)
 //   {
 //       // Allow stencilling
 //       HRESULT hr = __SetRenderState(D3DRENDERSTATE_STENCILENABLE, enabled);
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error enabling / disabling stencilling.",
 //           "D3DRenderSystem::setStencilCheckEnabled");

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setStencilBufferParams(CompareFunction func, 
 //       uint32 refValue, uint32 mask, StencilOperation stencilFailOp, 
 //       StencilOperation depthFailOp, StencilOperation passOp, 
 //       bool twoSidedOperation)
 //   {
 //       HRESULT hr;

 //       // D3D7 does not support 2-sided stencil operations
 //       if (twoSidedOperation)
 //           OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Direct3D7 does not support 2-sided stencil ops",
 //              "D3DRenderSystem::setStencilBufferParams");

 //       // Function
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILFUNC, 
 //           convertCompareFunction(func));

 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer test function.",
 //           "D3DRenderSystem::setStencilBufferParams");

 //       // reference value
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILREF, refValue);
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer reference value.",
 //           "D3DRenderSystem::setStencilBufferParams");

 //       // mask
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILMASK, mask);
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer mask.",
 //           "D3DRenderSystem::setStencilBufferParams");

 //       // fail op
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILFAIL, 
 //           convertStencilOp(stencilFailOp));
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil fail operation.",
 //           "D3DRenderSystem::setStencilBufferParams");

 //       // depth fail op
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILZFAIL, 
 //           convertStencilOp(depthFailOp));
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil depth fail operation.",
 //           "D3DRenderSystem::setStencilBufferParams");

 //       // pass op
 //       hr = __SetRenderState(D3DRENDERSTATE_STENCILPASS, 
 //           convertStencilOp(passOp));
 //       if (FAILED(hr))
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil pass operation.",
 //           "D3DRenderSystem::setStencilBufferParams");
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   D3DCMPFUNC D3DRenderSystem::convertCompareFunction(CompareFunction func)
 //   {
 //       switch(func)
 //       {
 //       case CMPF_ALWAYS_FAIL:
 //           return D3DCMP_NEVER;
 //       case CMPF_ALWAYS_PASS:
 //           return D3DCMP_ALWAYS;
 //       case CMPF_LESS:
 //           return D3DCMP_LESS;
 //       case CMPF_LESS_EQUAL:
 //           return D3DCMP_LESSEQUAL;
 //       case CMPF_EQUAL:
 //           return D3DCMP_EQUAL;
 //       case CMPF_NOT_EQUAL:
 //           return D3DCMP_NOTEQUAL;
 //       case CMPF_GREATER_EQUAL:
 //           return D3DCMP_GREATEREQUAL;
 //       case CMPF_GREATER:
 //           return D3DCMP_GREATER;
 //       };
 //       // to shut the compiler up
 //       return D3DCMP_ALWAYS;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   D3DSTENCILOP D3DRenderSystem::convertStencilOp(StencilOperation op)
 //   {
 //       switch(op)
 //       {
 //       case SOP_KEEP:
 //           return D3DSTENCILOP_KEEP;
 //       case SOP_ZERO:
 //           return D3DSTENCILOP_ZERO;
 //       case SOP_REPLACE:
 //           return D3DSTENCILOP_REPLACE;
 //       case SOP_INCREMENT:
 //           return D3DSTENCILOP_INCRSAT;
 //       case SOP_DECREMENT:
 //           return D3DSTENCILOP_DECRSAT;
 //       case SOP_INCREMENT_WRAP:
 //           return D3DSTENCILOP_INCR;
 //       case SOP_DECREMENT_WRAP:
 //           return D3DSTENCILOP_DECR;
 //       case SOP_INVERT:
 //           return D3DSTENCILOP_INVERT;
 //       };
 //       // To shut the compiler up
 //       return D3DSTENCILOP_KEEP;
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   DWORD D3DRenderSystem::_getCurrentAnisotropy(size_t unit)
 //   {
 //       DWORD oldVal;
 //       mlpD3DDevice->GetTextureStageState(unit, D3DTSS_MAXANISOTROPY, &oldVal);
 //       return oldVal;
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setTextureUnitFiltering(size_t unit, 
 //       FilterType ftype, FilterOptions filter)
 //   {
 //       __SetTextureStageState(unit, _getFilterCode(ftype), _getFilter(ftype, filter));
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setTextureLayerAnisotropy(size_t unit, unsigned int maxAnisotropy)
 //   {
 //       if ((DWORD)maxAnisotropy > m_D3DDeviceDesc.dwMaxAnisotropy)
 //           maxAnisotropy = m_D3DDeviceDesc.dwMaxAnisotropy;

 //       if (_getCurrentAnisotropy(unit) != maxAnisotropy)
 //           __SetTextureStageState( unit, D3DTSS_MAXANISOTROPY, maxAnisotropy );
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setVertexDeclaration(VertexDeclaration* decl)
 //   {
 //       // TODO
 //   }
	////////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setVertexBufferBinding(VertexBufferBinding* binding)
 //   {
 //       // TODO
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   D3DTEXTURESTAGESTATETYPE D3DRenderSystem::_getFilterCode(FilterType ft) const
 //   {
 //       switch (ft)
 //       {
 //       case FT_MIN:
 //           return D3DTSS_MINFILTER;
 //           break;
 //       case FT_MAG:
 //           return D3DTSS_MAGFILTER;
 //           break;
 //       case FT_MIP:
 //           return D3DTSS_MIPFILTER;
 //           break;
 //       }

 //       // to keep compiler happy
 //       return D3DTSS_MINFILTER;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   DWORD D3DRenderSystem::_getFilter(FilterType ft, FilterOptions fo) const
 //   {
 //       switch (ft)
 //       {
 //       case FT_MIN:
 //           switch( fo )
 //           {
 //               // NOTE: Fall through if device doesn't support requested type
 //           case FO_ANISOTROPIC:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )
 //               {
 //                   return D3DTFN_ANISOTROPIC;
 //                   break;
 //               }
 //           case FO_LINEAR:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )
 //               {
 //                   return D3DTFN_LINEAR;
 //                   break;
 //               }
 //           case FO_POINT:
 //           case FO_NONE:
 //               return D3DTFN_POINT;
 //               break;
 //           }
 //           break;
 //       case FT_MAG:
 //           switch( fo )
 //           {
 //           // NOTE: Fall through if device doesn't support requested type
 //           case FO_ANISOTROPIC:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC )
 //               {
 //                   return D3DTFG_ANISOTROPIC;
 //                   break;
 //               }
 //           case FO_LINEAR:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR )
 //               {
 //                   return D3DTFG_LINEAR;
 //                   break;
 //               }
 //           case FO_POINT:
 //           case FO_NONE:
 //               return D3DTFG_POINT;
 //               break;
 //           }
 //           break;
 //       case FT_MIP:
 //           switch( fo )
 //           {
 //           case FO_ANISOTROPIC:
 //           case FO_LINEAR:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR )
 //               {
 //                   return D3DTFP_LINEAR;
 //                   break;
 //               }
 //           case FO_POINT:
 //               if( m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR )
 //               {
 //                   return D3DTFP_POINT;
 //                   break;
 //               }
 //           case FO_NONE:
 //               return D3DTFP_NONE;
 //               break;
 //           }
 //           break;
 //       }

 //       // should never get here
 //       return 0;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setNormaliseNormals(bool normalise)
 //   {
 //       __SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, 
 //           normalise ? TRUE : FALSE);
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   HRESULT D3DRenderSystem::__SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
 //   {
 //       HRESULT hr;
 //       DWORD oldVal;

 //       if ( FAILED( hr = mlpD3DDevice->GetRenderState(state, &oldVal) ) )
 //           return hr;
 //       if ( oldVal == value )
 //           return D3D_OK;
 //       else
 //           return mlpD3DDevice->SetRenderState(state, value);
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   HRESULT D3DRenderSystem::__SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
 //   {
 //       HRESULT hr;
 //       DWORD oldVal;

 //       if ( FAILED( hr = mlpD3DDevice->GetTextureStageState(stage, type, &oldVal) ) )
 //           return hr;
 //       if ( oldVal == value )
 //           return D3D_OK;
 //       else
 //           return mlpD3DDevice->SetTextureStageState(stage, type, value);
 //   }

 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::clearFrameBuffer(unsigned int buffers, 
 //       const ColourValue& colour, Real depth, unsigned short stencil)
 //   {
	//	if (m_deviceLost)
	//		return;

	//	DWORD flags = 0;
 //       if (buffers & FBT_COLOUR)
 //       {
 //           flags |= D3DCLEAR_TARGET;
 //       }
 //       if (buffers & FBT_DEPTH)
 //       {
 //           flags |= D3DCLEAR_ZBUFFER;
 //       }
 //       // Only try to clear the stencil if supported, otherwise it will fail
 //       if (buffers & FBT_STENCIL && m_capabilities->hasCapability(RSC_HWSTENCIL))
 //       {
 //           flags |= D3DCLEAR_STENCIL;
 //       }
 //       HRESULT hr;
 //       if( FAILED( hr = mlpD3DDevice->Clear( 
 //           0, 
 //           NULL, 
 //           flags,
 //           colour.getAsARGB(), 
 //           depth, 
 //           stencil ) ) )
 //       {
 //           String msg = getErrorDescription(hr);
 //           OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error clearing frame buffer : " 
 //               + msg, "D3DRenderSystem::clearFrameBuffer" );
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_makeProjectionMatrix(Real left, Real right, 
 //       Real bottom, Real top, Real nearPlane, Real farPlane, Matrix4& dest,
 //       bool forGpuProgram)
 //   {
 //       Real width = right - left;
 //       Real height = top - bottom;
 //       Real q, qn;
 //       if (farPlane == 0)
 //       {
 //           q = 1 - Frustum::INFINITE_FAR_PLANE_ADJUST;
 //           qn = nearPlane * (Frustum::INFINITE_FAR_PLANE_ADJUST - 1);
 //       }
 //       else
 //       {
 //           q = farPlane / ( farPlane - nearPlane );
 //           qn = -q * nearPlane;
 //       }
 //       dest = Matrix4::ZERO;
 //       dest[0][0] = 2 * nearPlane / width;
 //       dest[0][2] = (right+left) / width;
 //       dest[1][1] = 2 * nearPlane / height;
 //       dest[1][2] = (top+bottom) / height;
 //       if (forGpuProgram)
 //       {
 //           dest[2][2] = -q;
 //           dest[3][2] = -1.0f;
 //       }
 //       else
 //       {
 //           dest[2][2] = q;
 //           dest[3][2] = 1.0f;
 //       }
 //       dest[2][3] = qn;
 //   }

 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setClipPlanes(const PlaneList& clipPlanes)
 //   {
 //       size_t i;
 //       size_t numClipPlanes;
 //       D3DVALUE dx7ClipPlane[4];
 //       DWORD mask = 0;
 //       HRESULT hr;
 //       numClipPlanes = clipPlanes.size();

 //       for (i = 0; i < numClipPlanes; ++i)
 //       {
 //           const Plane& plane = clipPlanes[i];

 //           dx7ClipPlane[0] = plane.normal.x;
 //           dx7ClipPlane[1] = plane.normal.y;
 //           dx7ClipPlane[2] = plane.normal.z;
 //           dx7ClipPlane[3] = plane.d;

 //           hr = mlpD3DDevice->SetClipPlane(i, dx7ClipPlane);
 //           if (FAILED(hr))
 //           {
 //               OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unable to set clip plane", 
 //                   "D3D7RenderSystem::setClipPlanes");
 //           }

 //           mask |= (1 << i);
 //       }

 //       hr = mlpD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, mask);
 //       if (FAILED(hr))
 //       {
 //           OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unable to set render state for clip planes", 
 //               "D3D7RenderSystem::setClipPlanes");
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::setClipPlane (ushort index, Real A, Real B, Real C, Real D)
 //   {
 //       float plane[4] = { A, B, C, D };
 //       mlpD3DDevice->SetClipPlane (index, plane);
 //   }

 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::enableClipPlane (ushort index, bool enable)
 //   {
 //       DWORD prev;
 //       mlpD3DDevice->GetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, &prev);
 //       __SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, prev | (1 << index));
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   HardwareOcclusionQuery* D3DRenderSystem::createHardwareOcclusionQuery()
 //   {
 //       return (HardwareOcclusionQuery*) 0;	// Hardware occlusion is not supported when DirectX7 is used
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
 //   {
 //       // D3D7 does not have a colour write mask setting, so emulate it using
 //       // scene blending, on the assumption that setSceneBlend will be called
 //       // before this
 //       if (red || green || blue || alpha)
 //       {
 //           // restore saved scene blend
 //           _setSceneBlending(mSavedSrcFactor, mSavedDestFactor);
 //       }
 //       else
 //       {
 //           _setSceneBlending(SBF_ZERO, SBF_ONE);
 //       }
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   Real D3DRenderSystem::getHorizontalTexelOffset(void)
 //   {
 //       // D3D considers the origin to be in the center of a pixel
 //       return -0.5f;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   Real D3DRenderSystem::getVerticalTexelOffset(void)
 //   {
 //       // D3D considers the origin to be in the center of a pixel
 //       return -0.5f;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   void D3DRenderSystem::_applyObliqueDepthProjection( Matrix4& _matrix, 
 //       const Plane& _plane, bool _forGpuProgram )
 //   {
	//	// Thanks to Eric Lenyel for posting this calculation at www.terathon.com

	//	// Calculate the clip-space corner point opposite the clipping plane
	//	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	//	// transform it into camera space by multiplying it
	//	// by the inverse of the projection matrix

	//	/* generalised version
	//	Vector4 q = matrix.inverse() * 
	//	Vector4(Math::Sign(plane.normal.x), Math::Sign(plane.normal.y), 1.0f, 1.0f);
	//	*/
	//	Vector4 q;
	//	q.x = Math::Sign(plane.normal.x) / matrix[0][0];
	//	q.y = Math::Sign(plane.normal.y) / matrix[1][1];
	//	q.z = 1.0F; 
	//	// flip the next bit from Lengyel since we're right-handed
	//	if (forGpuProgram)
	//	{
	//		q.w = (1.0F - matrix[2][2]) / matrix[2][3];
	//	}
	//	else
	//	{
	//		q.w = (1.0F + matrix[2][2]) / matrix[2][3];
	//	}

	//	// Calculate the scaled plane vector
	//	Vector4 clipPlane4d(plane.normal.x, plane.normal.y, plane.normal.z, plane.d);
	//	Vector4 c = clipPlane4d * (1.0F / (clipPlane4d.dotProduct(q)));

	//	// Replace the third row of the projection matrix
	//	matrix[2][0] = c.x;
	//	matrix[2][1] = c.y;
	//	// flip the next bit from Lengyel since we're right-handed
	//	if (forGpuProgram)
	//	{
	//		matrix[2][2] = c.z; 
	//	}
	//	else
	//	{
	//		matrix[2][2] = -c.z; 
	//	}
	//	matrix[2][3] = c.w;        

 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   Real D3DRenderSystem::getMinimumDepthInputValue(void)
 //   {
 //       // Range [0.0f, 1.0f]
 //       return 0.0f;
 //   }
 //   //////////////////////////////////////////////////////////////////////////
 //   Real D3DRenderSystem::getMaximumDepthInputValue(void)
 //   {
 //       // Range [0.0f, 1.0f]
 //       // D3D inverts even identity view matrices, so maximum INPUT is -1.0
 //       return -1.0f;
 //   }
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setPointSpritesEnabled( bool _enabled )
	//{
	//	if ( _enabled )
	//	{
	//		//__SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	//	}
	//	else
	//	{
	//		//__SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setPointParameters( float _size, bool _attenuationEnabled, 
	//	float _constant, float _linear, float _quadratic, float _minSize, float _maxSize )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTexture( std::size_t _stage, bool _enabled, const Texture* _tex )
	//{
	//	HRESULT hr;
	//	D3DTexture* dt = _tex;
	//	if ( _enabled && dt != NULL )
	//	{
	//		// note used
	//		dt->touch();

	//		LPDIRECTDRAWSURFACE7 pTex = dt->getDDSurface();
	//		if( m_texStageDesc[_stage].pTex != pTex )
	//		{
	//			hr = mlpD3DDevice->SetTexture( _stage, pTex );
	//			if( hr != S_OK )
	//			{
	//				String str = "Unable to set texture '" + tex->getName() + "' in D3D7";
	//				OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, str, "D3D7RenderSystem::_setTexture" );
	//			}

	//			// set stage desc.
	//			m_texStageDesc[_stage].pTex = pTex;
	//			//mTexStageDesc[stage].texType = D3D9Mappings::get(dt->getTextureType());
	//			m_texStageDesc[_stage].texType = D3D_TEX_TYPE_NORMAL;
	//		}
	//	}
	//	else
	//	{
	//		if (m_texStageDesc[_stage].pTex != 0)
	//		{
	//			hr = mlpD3DDevice->SetTexture( _stage, 0);
	//			if( hr != S_OK )
	//			{
	//				String str = "Unable to disable texture '" + StringConverter::toString(stage) + "' in D3D7";
	//				OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, str, "D3D7RenderSystem::_setTexture" );
	//			}
	//		}

	//		hr = this->__SetTextureStageState( _stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
	//		if( hr != S_OK )
	//		{
	//			String str = "Unable to disable texture '" + StringConverter::toString(stage) + "' in D3D7";
	//			OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, str, "D3D7RenderSystem::_setTexture" );
	//		}

	//		// set stage desc. to defaults
	//		m_texStageDesc[_stage].pTex = 0;
	//		m_texStageDesc[_stage].autoTexCoordType = TEXCALC_NONE;
	//		m_texStageDesc[_stage].coordIndex = 0;
	//		m_texStageDesc[_stage].texType = D3D_TEX_TYPE_NORMAL;
	//		//mTexStageDesc[stage].texType = D3D9Mappings::D3D_TEX_TYPE_NORMAL;
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureAddressingMode(size_t unit, const TextureUnitState::UVWAddressingMode& uvw)
	//{
	//	/*HRESULT hr;
	//	if( FAILED( hr = __SetSamplerState( stage, D3DSAMP_ADDRESSU, D3D9Mappings::get(uvw.u, mCaps) ) ) )
	//		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Failed to set texture addressing mode for U", "D3D9RenderSystem::_setTextureAddressingMode" );
	//	if( FAILED( hr = __SetSamplerState( stage, D3DSAMP_ADDRESSV, D3D9Mappings::get(uvw.v, mCaps) ) ) )
	//		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Failed to set texture addressing mode for V", "D3D9RenderSystem::_setTextureAddressingMode" );
	//	if( FAILED( hr = __SetSamplerState( stage, D3DSAMP_ADDRESSW, D3D9Mappings::get(uvw.w, mCaps) ) ) )
	//		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Failed to set texture addressing mode for W", "D3D9RenderSystem::_setTextureAddressingMode" );*/
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureBorderColour( std::size_t _unit, const ColourValue& _colour )
	//{
	//	/*HRESULT hr;
	//	if( FAILED( hr = __SetSamplerState( stage, D3DSAMP_BORDERCOLOR, colour.getAsARGB()) ) )
	//		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Failed to set texture border colour", "D3D9RenderSystem::_setTextureBorderColour" );*/
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setTextureMipmapBias( std::size_t _unit, float _bias )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_setDepthBias( float _constantBias, float _slopeScaleBias )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//VertexElementType D3DRenderSystem::getColourVertexElementType() const
	//{
	//	return VET_COLOUR_ARGB;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::_convertProjectionMatrix( const Matrix4& _matrix, Matrix4& _dest, bool _forGpuProgram )
	//{
	//	dest = _matrix;

	//	// Convert depth range from [-1,+1] to [0,1]
	//	_dest[2][0] = (_dest[2][0] + _dest[3][0]) / 2;
	//	_dest[2][1] = (_dest[2][1] + _dest[3][1]) / 2;
	//	_dest[2][2] = (_dest[2][2] + _dest[3][2]) / 2;
	//	_dest[2][3] = (_dest[2][3] + _dest[3][3]) / 2;

	//	if (!_forGpuProgram)
	//	{
	//		// Convert right-handed to left-handed
	//		_dest[0][2] = -_dest[0][2];
	//		_dest[1][2] = -_dest[1][2];
	//		_dest[2][2] = -_dest[2][2];
	//		_dest[3][2] = -_dest[3][2];
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::bindGpuProgramPassIterationParameters( GpuProgramType _gptype )
	//{

	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::preExtraThreadsStarted()
	//{
	//	// nothing to do - D3D9 shares rendering context already
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::postExtraThreadsStarted()
	//{
	//	// nothing to do - D3D9 shares rendering context already
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::registerThread()
	//{
	//	// nothing to do - D3D9 shares rendering context already
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::unregisterThread()
	//{
	//	// nothing to do - D3D9 shares rendering context already
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::destroyHardwareOcclusionQuery( HardwareOcclusionQuery* _hq )
	//{
	//	// HardwareOcclusionQuery not supported by Direct3D7
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::destroyRenderWindow( const String& _name )
	//{
	//	destroyRenderTarget( _name );
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::destroyRenderTexture( const String& _name )
	//{
	//	destroyRenderTarget( _name );
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::destroyRenderTarget( const String& _name )
	//{
	//	RenderTarget* rt = detachRenderTarget( _name );
	//	delete rt;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void D3DRenderSystem::attachRenderTarget( RenderTarget& _target )
	//{
	//	assert( _target.getPriority() < MENGE_NUM_RENDERTARGET_GROUPS );

	//	m_renderTargets.insert( TRenderTargetMap::value_type( _target.getName(), &_target ) );
	//	m_prioritisedRenderTargets.insert(
	//		RenderTargetPriorityMap::value_type( _target.getPriority(), &_target ));
	//}
	////////////////////////////////////////////////////////////////////////////
	//RenderTarget* D3DRenderSystem::getRenderTarget( const String& _name )
	//{
	//	TRenderTargetMap::iterator it = m_renderTargets.find( _name );
	//	RenderTarget *ret = NULL;

	//	if( it != m_renderTargets.end() )
	//	{
	//		ret = it->second;
	//	}

	//	return ret;
	//}
	////////////////////////////////////////////////////////////////////////////
	//bool D3DRenderSystem::getWaitForVerticalBlank() const
	//{
	//	return m_VSync;
	//}
	////////////////////////////////////////////////////////////////////////////
	void D3DRenderSystem::setWaitForVerticalBlank( bool _enabled )
	{
		m_VSync = _enabled;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
