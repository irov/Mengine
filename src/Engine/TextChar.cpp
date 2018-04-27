#include "TextChar.h"

#include "Core/String.h"

#include <algorithm>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const TVectorTextChunks::size_type TextChunkNPos = (TVectorTextChunks::size_type)(-1);
        //////////////////////////////////////////////////////////////////////////
        static TVectorCacheFonts::iterator find_font( TVectorCacheFonts & _cacheFonts, const TextFontInterfacePtr & _font )
        {
            for( TVectorCacheFonts::iterator
                it = _cacheFonts.begin(),
                it_end = _cacheFonts.end();
                it != it_end;
                ++it )
            {
                const CacheFont & cache = *it;

                if( cache.font != _font )
                {
                    continue;
                }

                return it;
            }

            return _cacheFonts.end();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool test2( TVectorTextChunks & _out, const U32String & _in, TVectorCacheFonts & _cacheFonts, uint32_t _font, const U32String & _pre, const U32String & _post, const U32String & _end, U32String::size_type _offset, U32String::size_type _size )
        {
            if( _offset == _size )
            {
                return true;
            }

            U32String::size_type found_pre = _in.find( _pre, _offset );

            if( found_pre == U32String::npos )
            {
                TextChunk c;
                c.value = _in.substr( _offset, _size - _offset );
                c.fontId = _font;

                _out.emplace_back( c );

                return true;
            }

            if( found_pre != _offset )
            {
                TextChunk c;
                c.value = _in.substr( _offset, found_pre - _offset );
                c.fontId = _font;

                _out.emplace_back( c );
            }

            U32String::size_type found_post = _in.find( _post, found_pre );

            if( found_post == std::string::npos )
            {
                return false;
            }

            U32String::size_type pre_size = _pre.size();
            U32String::size_type name_begin = found_pre + pre_size;
            U32String::size_type name_count = found_post - name_begin;
            U32String name = _in.substr( name_begin, name_count );

            U32String::size_type found_end = _in.find( _end, found_post );

            if( found_end == std::string::npos )
            {
                return false;
            }

            U32String::size_type new_begin = found_post + _post.size();
            U32String::size_type new_end = found_end;
            
            if( new_begin != new_end )
            {
                TextChunk c;
                c.value = _in.substr( new_begin, new_end - new_begin );

                ConstString c_FontName = Helper::stringizeString( name );

                TextFontInterfacePtr font = TEXT_SERVICE()
                    ->getFont( c_FontName );

                TVectorCacheFonts::iterator it_found = find_font( _cacheFonts, font );

                if( it_found != _cacheFonts.end() )
                {
                    c.fontId = std::distance( _cacheFonts.begin(), it_found );
                }
                else
                {
                    c.fontId = _cacheFonts.size();

                    CacheFont cache;
                    cache.font = font;

                    _cacheFonts.emplace_back( cache );
                }

                _out.emplace_back( c );
            }

            U32String::size_type end_size = _end.size();

            if( test2( _out, _in, _cacheFonts, _font, _pre, _post, _end, found_end + end_size, _size ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool test( TVectorTextChunks & _out, const U32String & _in, TVectorCacheFonts & _cacheFonts, uint32_t _font )
        {
            U32String tag_pre = U"<Font=";
            U32String tag_post = U">";
            U32String tag_end = U"</Font>";
            U32String::size_type offset = 0;
            U32String::size_type size = _in.size();

            bool successful = test2( _out, _in, _cacheFonts, _font, tag_pre, tag_post, tag_end, offset, size );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void substr( TVectorTextChunks & _out, const TVectorTextChunks & _str, TVectorTextChunks::size_type _offset, TVectorTextChunks::size_type _size )
        {
            TVectorTextChunks::const_iterator it_offset = _str.begin();
            std::advance( it_offset, _offset );

            TVectorTextChunks::const_iterator it_size = _str.begin();

            if( _size == TextChunkNPos )
            {
                it_size = _str.end();
            }
            else
            {
                std::advance( it_size, _offset + _size );
            }

            _out.insert( _out.begin(), it_offset, it_size );
        }
        //////////////////////////////////////////////////////////////////////////
        void split( TVectorTextLines & _lines, const TVectorTextChunks & _chunks, const TVectorU32String & _delims )
        {
            TVectorTextChunks collect;

            for( const TextChunk & chunk : _chunks )
            {
                TVectorU32String outStrings;
                if( Helper::u32split3( outStrings, chunk.value, _delims ) == false )
                { 
                    collect.emplace_back( chunk );

                    continue;
                }

                TVectorU32String::const_iterator it_strings = outStrings.begin();

                const U32String & first_value = *it_strings;

                TextChunk new_chunk;
                new_chunk.value = first_value;
                new_chunk.fontId = chunk.fontId;

                collect.emplace_back( new_chunk );

                ++it_strings;

                for( TVectorU32String::const_iterator
                    it = it_strings,
                    it_end = outStrings.end();
                    it != it_end;
                    ++it )
                {
                    const U32String & next_value = *it;

                    TextChunk next_chunk;
                    next_chunk.value = next_value;
                    next_chunk.fontId = chunk.fontId;

                    _lines.emplace_back( collect );
                    collect.clear();

                    collect.emplace_back( next_chunk );
                }
            }

            if( collect.empty() == false )
            {
                _lines.emplace_back( collect );
            }
        }
    }
};
