#	pragma once
#	include "ResourceReference.h"

namespace Menge
{
	class ResourceCursorICO
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceCursorICO )

	public:
		ResourceCursorICO();

	public:
		const WString& getPath() const;
		void * getBuffer( size_t & _size ) const;

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