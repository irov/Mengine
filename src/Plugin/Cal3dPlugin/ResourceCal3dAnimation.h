#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "cal3d/coreanimation.h"

namespace Menge
{
	class ResourceCal3dAnimation
		: public ResourceReference
	{
		RESOURCE_DECLARE(ResourceCal3dAnimation)

	public:
		ResourceCal3dAnimation();
		~ResourceCal3dAnimation();

	public:
		const CalCoreAnimationPtr & getAnimation() const;

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		FilePath m_filePath;

		CalCoreAnimationPtr m_animation;
	};
}
