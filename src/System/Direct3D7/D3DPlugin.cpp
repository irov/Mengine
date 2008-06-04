
#include "D3D7Plugin.h"
//#include "OgreRoot.h"

namespace Menge
{
	const String sPluginName = "D3D7 RenderSystem";
	//---------------------------------------------------------------------
	D3D7Plugin::D3D7Plugin()
		: mRenderSystem(0)
	{

	}
	//---------------------------------------------------------------------
	const String& D3D7Plugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void D3D7Plugin::install()
	{
		// Create the DirectX 7 rendering api
#ifdef OGRE_STATIC_LIB
		HINSTANCE hInst = GetModuleHandle( NULL );
#else
		HINSTANCE hInst = GetModuleHandle( "RenderSystem_Direct3D7.dll" );
#endif
		mRenderSystem = new D3DRenderSystem( hInst );
		// Register the render system
		Root::getSingleton().addRenderSystem( mRenderSystem );
	}
	//---------------------------------------------------------------------
	void D3D7Plugin::initialise()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void D3D7Plugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void D3D7Plugin::uninstall()
	{
		delete mRenderSystem;
		mRenderSystem = 0;
	}


}
