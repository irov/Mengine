#	pragma once

#	include "ResourceCursor.h"

namespace Menge
{
	class ResourceCursorICO
		: public ResourceCursor
	{
		DECLARE_VISITABLE( ResourceCursor );

	public:
		ResourceCursorICO();

	public:
		const FilePath & getPath() const override;
		const MemoryInterfacePtr & getBuffer() const override;

	public:
		bool _compile() override;
		void _release() override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		FilePath m_path;
				
		MemoryInterfacePtr m_buffer;
	};
}