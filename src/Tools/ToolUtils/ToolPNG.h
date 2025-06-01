#pragma once

#include <stdint.h>

bool is_png_buffer( const uint8_t * buffer, size_t size );
bool is_png_file( const wchar_t * _path );

uint8_t * png_load_from_memory( const uint8_t * _buffer, size_t _size, uint32_t * const _width, uint32_t * const _height, uint32_t * const _channels );
const uint8_t * png_write_to_memory( const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels, size_t * const _write );
const uint8_t * png_move_to_memory( const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels, size_t * const _write );

uint8_t * png_load_from_file( const wchar_t * _path, uint32_t * const _width, uint32_t * const _height, uint32_t * const _channels );
bool png_move_to_file( const wchar_t * _path, const uint8_t * _buffer, size_t _size, uint32_t _width, uint32_t _height, uint32_t _channels );