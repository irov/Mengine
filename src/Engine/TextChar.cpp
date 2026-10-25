#include "TextChar.h"

#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIterator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static void appendChunk( VectorTextLineChunks * const _chunks, const TextLineChunk & _chunk )
            {
                if( _chunks->empty() == false )
                {
                    if( _chunk.value.empty() == true )
                    {
                        return;
                    }

                    TextLineChunk & lastChunk = _chunks->back();

                    if( lastChunk.fontId == _chunk.fontId )
                    {
                        lastChunk.value.append( _chunk.value );

                        return;
                    }
                }

                _chunks->emplace_back( _chunk );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_CONSTEXPR VectorTextLineChunks::size_type TextChunkNPos = MENGINE_UNKNOWN_SIZE;
        //////////////////////////////////////////////////////////////////////////
        static bool test2( VectorTextLineChunks * const _out, const U32String & _in, VectorCacheFonts * const _cacheFonts, uint32_t _font, const U32String & _pre, const U32String & _post, const U32String & _end, U32String::size_type _offset, U32String::size_type _size )
        {
            if( _offset == _size )
            {
                return true;
            }

            U32String::size_type found_pre = _in.find( _pre, _offset );

            if( found_pre == U32String::npos )
            {
                TextLineChunk c;
                c.value = _in.substr( _offset, _size - _offset );
                c.fontId = _font;

                _out->emplace_back( c );

                return true;
            }

            if( found_pre != _offset )
            {
                TextLineChunk c;
                c.value = _in.substr( _offset, found_pre - _offset );
                c.fontId = _font;

                _out->emplace_back( c );
            }

            U32String::size_type found_post = _in.find( _post, found_pre );

            if( found_post == String::npos )
            {
                return false;
            }

            U32String::size_type pre_size = _pre.size();
            U32String::size_type name_begin = found_pre + pre_size;
            U32String::size_type name_count = found_post - name_begin;

            MENGINE_ASSERTION_FATAL( name_count <= 256, "text font name length %zu exceeds 256 characters"
                , name_count
            );

            U32String name = _in.substr( name_begin, name_count );

            U32String::size_type found_end = _in.find( _end, found_post );

            if( found_end == String::npos )
            {
                return false;
            }

            U32String::size_type new_begin = found_post + _post.size();
            U32String::size_type new_end = found_end;

            if( new_begin != new_end )
            {
                TextLineChunk c;
                c.value = _in.substr( new_begin, new_end - new_begin );

                Char buffer_name[256 + 1] = {'\0'};

                Char * buffer_name_iterator = buffer_name;
                for( U32String::value_type v : name )
                {
                    *buffer_name_iterator++ = (Char)v;
                }

                ConstString c_FontName = Helper::stringizeString( buffer_name );

                const FontInterfacePtr & font = FONT_SERVICE()
                    ->getFont( c_FontName );

                VectorCacheFonts::iterator it_found = StdAlgorithm::find_if( _cacheFonts->begin(), _cacheFonts->end(), [&font]( const CacheFont & cache )
                {
                    return cache.font == font;
                } );

                if( it_found != _cacheFonts->end() )
                {
                    ptrdiff_t d = StdIterator::distance( _cacheFonts->begin(), it_found );

                    c.fontId = (uint32_t)d;
                }
                else
                {
                    VectorCacheFonts::size_type cacheFontSize = _cacheFonts->size();

                    c.fontId = (uint32_t)cacheFontSize;

                    CacheFont cache;
                    cache.font = font;

                    _cacheFonts->emplace_back( cache );
                }

                _out->emplace_back( c );
            }

            U32String::size_type end_size = _end.size();

            if( test2( _out, _in, _cacheFonts, _font, _pre, _post, _end, found_end + end_size, _size ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool test( VectorTextLineChunks * const _out, const U32String & _in, VectorCacheFonts * const _cacheFonts, uint32_t _font )
        {
            U32String tag_pre = U"{{font=";
            U32String tag_post = U"}}";
            U32String tag_end = U"{{font}}";
            U32String::size_type offset = 0;
            U32String::size_type size = _in.size();

            bool successful = test2( _out, _in, _cacheFonts, _font, tag_pre, tag_post, tag_end, offset, size );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void substr( VectorTextLineChunks * const _out, const VectorTextLineChunks & _str, VectorTextLineChunks::size_type _offset, VectorTextLineChunks::size_type _size )
        {
            VectorTextLineChunks::const_iterator it_offset = _str.begin();
            StdIterator::advance( it_offset, _offset );

            VectorTextLineChunks::const_iterator it_size = _str.begin();

            if( _size == TextChunkNPos )
            {
                it_size = _str.end();
            }
            else
            {
                StdIterator::advance( it_size, _offset + _size );
            }

            _out->insert( _out->begin(), it_offset, it_size );
        }
        //////////////////////////////////////////////////////////////////////////
        void split( VectorTextLineChunks2 * const _lines, const VectorTextLineChunks & _chunks, const VectorU32String & _delims )
        {
            U32String text;

            for( const TextLineChunk & chunk : _chunks )
            {
                text.append( chunk.value );
            }

            VectorTextLineChunks collect;
            size_t begin = 0;
            size_t chunkEnd = 0;

            for( const TextLineChunk & chunk : _chunks )
            {
                size_t chunkBegin = chunkEnd;
                chunkEnd += chunk.value.size();

                size_t index = begin;

                while( index < chunkEnd )
                {
                    size_t delimiterSize = 0;

                    for( const U32String & delimiter : _delims )
                    {
                        size_t size = delimiter.size();

                        if( size <= delimiterSize )
                        {
                            continue;
                        }

                        if( text.compare( index, size, delimiter ) != 0 )
                        {
                            continue;
                        }

                        delimiterSize = size;
                    }

                    if( delimiterSize == 0 )
                    {
                        ++index;

                        continue;
                    }

                    TextLineChunk lineChunk;
                    lineChunk.value = chunk.value.substr( begin - chunkBegin, index - begin );
                    lineChunk.fontId = chunk.fontId;

                    Detail::appendChunk( &collect, lineChunk );

                    _lines->emplace_back( collect );
                    collect.clear();

                    index += delimiterSize;
                    begin = index;
                }

                if( begin > chunkEnd )
                {
                    continue;
                }

                TextLineChunk tailChunk;
                tailChunk.value = chunk.value.substr( begin - chunkBegin );
                tailChunk.fontId = chunk.fontId;

                Detail::appendChunk( &collect, tailChunk );

                begin = chunkEnd;
            }

            if( collect.empty() == false )
            {
                _lines->emplace_back( collect );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
};
