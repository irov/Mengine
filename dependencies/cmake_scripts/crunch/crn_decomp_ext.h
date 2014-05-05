#	pragma once

#	include <stdint.h>

namespace crnex
{
	bool valid_crn_header( void * _buffer );

	uint32_t get_crn_header_faces( void * _buffer );
	uint32_t get_crn_header_width( void * _buffer );
	uint32_t get_crn_header_height( void * _buffer );
	bool is_crn_header_format_dxt5( void * _buffer );
	bool is_crn_header_format_dxt1( void * _buffer );
	uint32_t get_crn_header_data_size( void * _buffer );

	bool decode_crn( void * _buffer, size_t _capacity, const void * _source, size_t _size, uint32_t _pitch );
}