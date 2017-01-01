#	include "crn_decomp_ext.h"

#	include <stdio.h>

#	include "inc/crn_decomp.h"

namespace crnex
{
	static const crnd::crn_header * s_get_crn_header( void * _buffer )
	{
		const crnd::crn_header * header = static_cast<const crnd::crn_header *>(_buffer);

		return header;
	}

	bool valid_crn_header( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		if( header->m_sig != crnd::crn_header::cCRNSigValue )
		{
			return false;
		}

		if( header->m_header_size != sizeof(crnd::crn_header) )
		{
			return false;
		}

		return true;
	}

	uint32_t get_crn_header_faces( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_faces;
	}

	uint32_t get_crn_header_width( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_width;
	}

	uint32_t get_crn_header_height( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_height;
	}

	bool is_crn_header_format_dxt5( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_format == cCRNFmtDXT5;
	}

	bool is_crn_header_format_dxt1( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_format == cCRNFmtDXT1;
	}

	uint32_t get_crn_header_data_size( void * _buffer )
	{
		const crnd::crn_header * header = s_get_crn_header( _buffer );

		return header->m_data_size;
	}

	bool decode_crn( void * _buffer, size_t _capacity, const void * _source, size_t _size, uint32_t _pitch )
	{
		crnd::crnd_unpack_context pContext = crnd::crnd_unpack_begin( _source, _size );

		if( pContext == nullptr )
		{
			return false;
		}

		void* pFaces[1];
		pFaces[0] = _buffer;

		bool successful = crnd::crnd_unpack_level( pContext, pFaces, _capacity, _pitch, 0 );

		crnd::crnd_unpack_end( pContext );

		return successful;
	}
}
