#pragma once

#include "Kernel/Node.h"

#include "Kernel/Soundable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class SurfaceSound> SurfaceSoundPtr;
    //////////////////////////////////////////////////////////////////////////
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
