#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "Kernel/Node.h"

#	include "Kernel/Animatable.h"
#	include "Kernel/Soundable.h"

#   include "ResourceSound.h"
#	include "SurfaceSound.h"

namespace Menge
{
	class SoundEmitter
		: public Node
	{
	public:
		SoundEmitter();
		~SoundEmitter();

	public:
		void setSurfaceSound( const SurfaceSoundPtr & _surfaceSound );
		const SurfaceSoundPtr & getSurfaceSound() const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		SurfaceSoundPtr m_surfaceSound;
	};
};
