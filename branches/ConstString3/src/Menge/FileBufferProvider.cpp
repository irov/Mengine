#	include "FileBufferProvider.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TBlobject * FileBufferProvider::getBuffer()
	{
		if( m_free.empty() == false )
		{
			m_buffers.splice( m_buffers.begin(), m_free, m_free.begin() );

			TBuffers::iterator it_buffer = m_buffers.begin();

			TBlobject & blobject = *it_buffer;
			blobject.clear();

			return &blobject;
		}

		TBlobject blob;
		TBuffers::iterator it_insert = m_buffers.insert( m_buffers.begin(), blob );

		return &*it_insert;
	}
	namespace
	{
		class FBufferFind
		{
		public:
			FBufferFind( const TBlobject * _buffer )
				: m_buffer(_buffer)
			{
			}

		public:
			bool operator() ( const TBlobject & _buffer )
			{
				return m_buffer == &_buffer;
			}

		protected:
			const TBlobject * m_buffer;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void FileBufferProvider::releaseBuffer( TBlobject * _buffer )
	{
		TBuffers::iterator it_buffer = std::find_if( m_buffers.begin(), m_buffers.end(), FBufferFind(_buffer) );
		
		m_free.splice( m_free.begin(), m_buffers, it_buffer );
	}
}