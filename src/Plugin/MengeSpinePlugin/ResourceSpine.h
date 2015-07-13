#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "spine/spine.h"

namespace Menge
{
	class ResourceSpine
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSpine )

	public:
		ResourceSpine();
		~ResourceSpine();

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		FilePath m_filePath;
	};
}
