#	pragma once

//#	include "OgrePlugin.h"
#	include "D3D7RenderSystem.h"

namespace Menge
{

	/** Plugin instance for D3D9 Manager */
	class D3D7Plugin : public Plugin
	{
	public:
		D3D7Plugin();


		/// @copydoc Plugin::getName
		const String& getName() const;

		/// @copydoc Plugin::install
		void install();

		/// @copydoc Plugin::initialise
		void initialise();

		/// @copydoc Plugin::shutdown
		void shutdown();

		/// @copydoc Plugin::uninstall
		void uninstall();
	protected:
		D3DRenderSystem* mRenderSystem;
	};
}

#endif
