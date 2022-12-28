#include "SHA1.h"

#include "Kernel/Base64.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#define SHA1_DIGEST_SIZE 20

#define SHA1_ROLL Detail::SHA1_rotl32
#define SHA1_BLK0(i) (block->l[i] = (SHA1_ROLL(block->l[i],24)&0xFF00FF00) | (SHA1_ROLL(block->l[i],8)&0x00FF00FF))
#define SHA1_BLK(i) (block->l[i&15] = SHA1_ROLL(block->l[(i+13)&15]^block->l[(i+8)&15]^block->l[(i+2)&15]^block->l[i&15],1))

#define SHA1_R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+SHA1_BLK0(i)+0x5A827999+SHA1_ROLL(v,5);w=SHA1_ROLL(w,30);
#define SHA1_R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+SHA1_BLK(i)+0x5A827999+SHA1_ROLL(v,5);w=SHA1_ROLL(w,30);
#define SHA1_R2(v,w,x,y,z,i) z+=(w^x^y)+SHA1_BLK(i)+0x6ED9EBA1+SHA1_ROLL(v,5);w=SHA1_ROLL(w,30);
#define SHA1_R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+SHA1_BLK(i)+0x8F1BBCDC+SHA1_ROLL(v,5);w=SHA1_ROLL(w,30);
#define SHA1_R4(v,w,x,y,z,i) z+=(w^x^y)+SHA1_BLK(i)+0xCA62C1D6+SHA1_ROLL(v,5);w=SHA1_ROLL(w,30);

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            struct SHA1_CTX
            {
                uint32_t state[5];
                size_t count[2];
                uint8_t  buffer[64];
            };
            //////////////////////////////////////////////////////////////////////////
            static uint32_t SHA1_rotl32( uint32_t var, uint32_t hops )
            {
                return (var << hops) | (var >> (32 - hops));
            }
            //////////////////////////////////////////////////////////////////////////
            static void SHA1_Transform( uint32_t state[5], const uint8_t buffer[64] )
            {
                typedef union
                {
                    uint8_t c[64];
                    uint32_t l[16];
                } CHAR64LONG16;

                CHAR64LONG16 * block;

                block = (CHAR64LONG16 *)buffer;

                uint32_t a = state[0];
                uint32_t b = state[1];
                uint32_t c = state[2];
                uint32_t d = state[3];
                uint32_t e = state[4];

                SHA1_R0( a, b, c, d, e, 0 ); SHA1_R0( e, a, b, c, d, 1 ); SHA1_R0( d, e, a, b, c, 2 ); SHA1_R0( c, d, e, a, b, 3 );
                SHA1_R0( b, c, d, e, a, 4 ); SHA1_R0( a, b, c, d, e, 5 ); SHA1_R0( e, a, b, c, d, 6 ); SHA1_R0( d, e, a, b, c, 7 );
                SHA1_R0( c, d, e, a, b, 8 ); SHA1_R0( b, c, d, e, a, 9 ); SHA1_R0( a, b, c, d, e, 10 ); SHA1_R0( e, a, b, c, d, 11 );
                SHA1_R0( d, e, a, b, c, 12 ); SHA1_R0( c, d, e, a, b, 13 ); SHA1_R0( b, c, d, e, a, 14 ); SHA1_R0( a, b, c, d, e, 15 );
                SHA1_R1( e, a, b, c, d, 16 ); SHA1_R1( d, e, a, b, c, 17 ); SHA1_R1( c, d, e, a, b, 18 ); SHA1_R1( b, c, d, e, a, 19 );
                SHA1_R2( a, b, c, d, e, 20 ); SHA1_R2( e, a, b, c, d, 21 ); SHA1_R2( d, e, a, b, c, 22 ); SHA1_R2( c, d, e, a, b, 23 );
                SHA1_R2( b, c, d, e, a, 24 ); SHA1_R2( a, b, c, d, e, 25 ); SHA1_R2( e, a, b, c, d, 26 ); SHA1_R2( d, e, a, b, c, 27 );
                SHA1_R2( c, d, e, a, b, 28 ); SHA1_R2( b, c, d, e, a, 29 ); SHA1_R2( a, b, c, d, e, 30 ); SHA1_R2( e, a, b, c, d, 31 );
                SHA1_R2( d, e, a, b, c, 32 ); SHA1_R2( c, d, e, a, b, 33 ); SHA1_R2( b, c, d, e, a, 34 ); SHA1_R2( a, b, c, d, e, 35 );
                SHA1_R2( e, a, b, c, d, 36 ); SHA1_R2( d, e, a, b, c, 37 ); SHA1_R2( c, d, e, a, b, 38 ); SHA1_R2( b, c, d, e, a, 39 );
                SHA1_R3( a, b, c, d, e, 40 ); SHA1_R3( e, a, b, c, d, 41 ); SHA1_R3( d, e, a, b, c, 42 ); SHA1_R3( c, d, e, a, b, 43 );
                SHA1_R3( b, c, d, e, a, 44 ); SHA1_R3( a, b, c, d, e, 45 ); SHA1_R3( e, a, b, c, d, 46 ); SHA1_R3( d, e, a, b, c, 47 );
                SHA1_R3( c, d, e, a, b, 48 ); SHA1_R3( b, c, d, e, a, 49 ); SHA1_R3( a, b, c, d, e, 50 ); SHA1_R3( e, a, b, c, d, 51 );
                SHA1_R3( d, e, a, b, c, 52 ); SHA1_R3( c, d, e, a, b, 53 ); SHA1_R3( b, c, d, e, a, 54 ); SHA1_R3( a, b, c, d, e, 55 );
                SHA1_R3( e, a, b, c, d, 56 ); SHA1_R3( d, e, a, b, c, 57 ); SHA1_R3( c, d, e, a, b, 58 ); SHA1_R3( b, c, d, e, a, 59 );
                SHA1_R4( a, b, c, d, e, 60 ); SHA1_R4( e, a, b, c, d, 61 ); SHA1_R4( d, e, a, b, c, 62 ); SHA1_R4( c, d, e, a, b, 63 );
                SHA1_R4( b, c, d, e, a, 64 ); SHA1_R4( a, b, c, d, e, 65 ); SHA1_R4( e, a, b, c, d, 66 ); SHA1_R4( d, e, a, b, c, 67 );
                SHA1_R4( c, d, e, a, b, 68 ); SHA1_R4( b, c, d, e, a, 69 ); SHA1_R4( a, b, c, d, e, 70 ); SHA1_R4( e, a, b, c, d, 71 );
                SHA1_R4( d, e, a, b, c, 72 ); SHA1_R4( c, d, e, a, b, 73 ); SHA1_R4( b, c, d, e, a, 74 ); SHA1_R4( a, b, c, d, e, 75 );
                SHA1_R4( e, a, b, c, d, 76 ); SHA1_R4( d, e, a, b, c, 77 ); SHA1_R4( c, d, e, a, b, 78 ); SHA1_R4( b, c, d, e, a, 79 );

                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
                state[4] += e;
            }
            //////////////////////////////////////////////////////////////////////////
            static void SHA1_Init( SHA1_CTX * context )
            {
                context->state[0] = 0x67452301;
                context->state[1] = 0xEFCDAB89;
                context->state[2] = 0x98BADCFE;
                context->state[3] = 0x10325476;
                context->state[4] = 0xC3D2E1F0;
                context->count[0] = 0;
                context->count[1] = 0;
            }
            //////////////////////////////////////////////////////////////////////////
            static void SHA1_Update( SHA1_CTX * context, const uint8_t * data, size_t len )
            {
                size_t j = (context->count[0] >> 3) & 63;

                if( (context->count[0] += len << 3) < (len << 3) )
                {
                    context->count[1]++;
                }

                context->count[1] += (len >> 29);

                size_t i;

                if( (j + len) > 63 )
                {
                    MENGINE_MEMCPY( &context->buffer[j], data, (i = 64 - j) );

                    SHA1_Transform( context->state, context->buffer );

                    for( ; i + 63 < len; i += 64 )
                    {
                        SHA1_Transform( context->state, data + i );
                    }

                    j = 0;
                }
                else
                {
                    i = 0;
                }

                MENGINE_MEMCPY( &context->buffer[j], &data[i], len - i );
            }
            //////////////////////////////////////////////////////////////////////////
            static void SHA1_Final( SHA1_CTX * context, uint8_t * const digest, uint32_t _size )
            {
                uint8_t finalcount[8];

                for( uint32_t i = 0; i != 8; i++ )
                {
                    finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
                }

                SHA1_Update( context, (uint8_t *)"\200", 1 );

                while( (context->count[0] & 504) != 448 )
                {
                    SHA1_Update( context, (uint8_t *)"\0", 1 );
                }

                SHA1_Update( context, finalcount, 8 );

                for( uint32_t i = 0; i != _size; i++ )
                {
                    digest[i] = (uint8_t)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
                }

                MENGINE_MEMSET( context->buffer, 0, 64 );
                MENGINE_MEMSET( context->state, 0, 20 );
                MENGINE_MEMSET( context->count, 0, 8 );
                MENGINE_MEMSET( finalcount, 0, 8 );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1( const void * _buffer, size_t _size, uint8_t * const _sha1 )
        {
            Detail::SHA1_CTX context;

            Detail::SHA1_Init( &context );
            Detail::SHA1_Update( &context, (uint8_t *)_buffer, _size );
            Detail::SHA1_Final( &context, _sha1, SHA1_DIGEST_SIZE );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1u64( const void * _buffer, size_t _size, uint64_t * const _u64 )
        {
            Detail::SHA1_CTX context;

            Detail::SHA1_Init( &context );
            Detail::SHA1_Update( &context, (uint8_t *)_buffer, _size );
            Detail::SHA1_Final( &context, (uint8_t *)_u64, sizeof( uint64_t ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1HEX( const void * _buffer, size_t _size, Char * const _hex )
        {
            uint8_t sha1[SHA1_DIGEST_SIZE];
            Helper::makeSHA1( _buffer, _size, sha1 );

            Helper::encodeHexadecimal( sha1, sizeof( sha1 ), _hex, ~0U, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1String( const Char * _string, Char * const _hex )
        {
            size_t len = MENGINE_STRLEN( _string );

            uint8_t sha1[SHA1_DIGEST_SIZE];
            Helper::makeSHA1( _string, len, sha1 );

            Helper::encodeHexadecimal( sha1, sizeof( sha1 ), _hex, ~0U, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1Base64( const void * _buffer, size_t _size, Char * const _base64 )
        {
            uint8_t sha1[SHA1_DIGEST_SIZE];
            Helper::makeSHA1( _buffer, _size, sha1 );

            Helper::encodeBase64( sha1, sizeof( sha1 ), true, _base64, ~0U, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
