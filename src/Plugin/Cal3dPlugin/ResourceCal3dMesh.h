#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "cal3d/coremesh.h"

namespace Menge
{
	class ResourceCal3dMesh
		: public ResourceReference
	{
		RESOURCE_DECLARE(ResourceCal3dMesh)

	public:
		ResourceCal3dMesh();
		~ResourceCal3dMesh();

	public:
		const CalCoreMeshPtr & getMesh() const;

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		FilePath m_filePath;

		CalCoreMeshPtr m_mesh;
	};
}
