#	pragma once

#	include "ResourceCursor.h"

namespace Menge
{
	class ResourceCursorSystem
		: public ResourceCursor
	{
		RESOURCE_DECLARE( ResourceCursorSystem )

	public:
		ResourceCursorSystem();

	public:
		const FilePath & getPath() const override;
		const MemoryInterfacePtr & getBuffer() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		FilePath m_path;

		MemoryInterfacePtr m_buffer;
	};
}