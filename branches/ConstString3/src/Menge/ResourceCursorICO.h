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
		const WString& getPath() const override;
		void * getBuffer( size_t & _size ) const override;

	public:
		bool _compile() override;
		void _release() override;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		WString m_path;
				
		char * m_buffer;
		size_t m_bufferSize;
	};
}