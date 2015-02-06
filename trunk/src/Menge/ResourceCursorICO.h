#	pragma once

#	include "ResourceCursor.h"

namespace Menge
{
	class ResourceCursorICO
		: public ResourceCursor
	{
		RESOURCE_DECLARE( ResourceCursorICO )

	public:
		ResourceCursorICO();

	public:
		const FilePath & getPath() const override;
		const Blobject & getBuffer() const override;

	public:
		bool _compile() override;
		void _release() override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		FilePath m_path;
				
		Blobject m_buffer;
	};
}