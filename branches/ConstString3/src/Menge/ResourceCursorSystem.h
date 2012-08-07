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
		const WString & getPath() const override;
		void * getBuffer( size_t & _size ) const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		WString m_path;	
	};
}