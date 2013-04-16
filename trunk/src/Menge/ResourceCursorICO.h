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
		void * getBuffer( size_t & _size ) const override;

	public:
		bool _compile() override;
		void _release() override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		FilePath m_path;
				
		char * m_buffer;
		size_t m_bufferSize;
	};
}