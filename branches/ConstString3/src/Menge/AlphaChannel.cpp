#	include "AlphaChannel.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AlphaChannel::AlphaChannel( size_t _width, size_t _height )
		: m_width(_width)
		, m_height(_height)
		, m_refcount(1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * AlphaChannel::createAlphaBuffer()
	{
		size_t mipmap_width = m_width;
		size_t mipmap_height = m_height;

		while( mipmap_width && mipmap_height )
		{
			unsigned char * buff = new unsigned char[mipmap_width * mipmap_height];

			memset( buff, 0, mipmap_width * mipmap_height );
						
			m_mipmap.push_back( buff );

			mipmap_width >>= 1;
			mipmap_height >>= 1;
		}

		return &m_mipmap[0][0];
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannel::makeMipMapLevel()
	{
		size_t mipmap_width = m_width;
		size_t mipmap_height = m_height;

		for( size_t
			it = 1,
			it_end = m_mipmap.size();
		it != it_end;
		++it )
		{
			size_t cur_width = mipmap_width >> 1;
			size_t cur_height = mipmap_height >> 1;

			unsigned char * prev_buff = m_mipmap[it - 1];
			unsigned char * cur_buff = m_mipmap[it];

			for( size_t i = 0; i != cur_width; ++i )
			{
				for( size_t j = 0; j != cur_height; ++j )
				{
					unsigned char b0 = std::max( prev_buff[ (i * 2 + 0) + (j * 2 + 0) * mipmap_width ], prev_buff[ (i * 2 + 1) + (j * 2 + 0) * mipmap_width ] );
					unsigned char b1 = std::max( prev_buff[ (i * 2 + 0) + (j * 2 + 1) * mipmap_width ], prev_buff[ (i * 2 + 1) + (j * 2 + 1) * mipmap_width ] );

					unsigned char b_max = std::max( b0, b1 );

					cur_buff[ i + j * cur_width ] = b_max;
				}
			}

			mipmap_width = cur_width;
			mipmap_height = cur_height;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AlphaChannel::releaseAlphaBuffer()
	{
		if( --m_refcount == 0 )
		{
			this->destroyAlphaBuffer();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannel::destroyAlphaBuffer()
	{
		for( TMipMapBuffers::iterator
			it = m_mipmap.begin(),
			it_end = m_mipmap.end();
		it != it_end;
		++it )
		{
			delete [] *it;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * AlphaChannel::getAlphaBuffer( size_t _level, size_t & _width, size_t & _height )
	{
		size_t max_level = m_mipmap.size() - 1;

		if( _level > max_level )
		{
			_width = m_width >> max_level;
			_height = m_height >> max_level;

			return &m_mipmap.back()[0];
		}

		++m_refcount;

		_width = m_width >> _level;
		_height = m_height >> _level;
		
		return &m_mipmap[_level][0];
	}
}	// namespace Menge
