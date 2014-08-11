#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "cal3d/coreskeleton.h"

namespace Menge
{
	class ResourceCal3dSkeleton
		: public ResourceReference
	{
		RESOURCE_DECLARE(ResourceCal3dSkeleton)

	public:
		ResourceCal3dSkeleton();
		~ResourceCal3dSkeleton();

	public:
		const CalCoreSkeletonPtr & getSkeleton() const;

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		FilePath m_filePath;

		CalCoreSkeletonPtr m_skeleton;
	};
}
