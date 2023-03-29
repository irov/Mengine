#include "ColorHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr float one_div_255[] = {
            0.f / 255.f, 1.f / 255.f, 2.f / 255.f, 3.f / 255.f, 4.f / 255.f, 5.f / 255.f, 6.f / 255.f, 7.f / 255.f, 8.f / 255.f, 9.f / 255.f,
            10.f / 255.f, 11.f / 255.f, 12.f / 255.f, 13.f / 255.f, 14.f / 255.f, 15.f / 255.f, 16.f / 255.f, 17.f / 255.f, 18.f / 255.f, 19.f / 255.f,
            20.f / 255.f, 21.f / 255.f, 22.f / 255.f, 23.f / 255.f, 24.f / 255.f, 25.f / 255.f, 26.f / 255.f, 27.f / 255.f, 28.f / 255.f, 29.f / 255.f,
            30.f / 255.f, 31.f / 255.f, 32.f / 255.f, 33.f / 255.f, 34.f / 255.f, 35.f / 255.f, 36.f / 255.f, 37.f / 255.f, 38.f / 255.f, 39.f / 255.f,
            40.f / 255.f, 41.f / 255.f, 42.f / 255.f, 43.f / 255.f, 44.f / 255.f, 45.f / 255.f, 46.f / 255.f, 47.f / 255.f, 48.f / 255.f, 49.f / 255.f,
            50.f / 255.f, 51.f / 255.f, 52.f / 255.f, 53.f / 255.f, 54.f / 255.f, 55.f / 255.f, 56.f / 255.f, 57.f / 255.f, 58.f / 255.f, 59.f / 255.f,
            60.f / 255.f, 61.f / 255.f, 62.f / 255.f, 63.f / 255.f, 64.f / 255.f, 65.f / 255.f, 66.f / 255.f, 67.f / 255.f, 68.f / 255.f, 69.f / 255.f,
            70.f / 255.f, 71.f / 255.f, 72.f / 255.f, 73.f / 255.f, 74.f / 255.f, 75.f / 255.f, 76.f / 255.f, 77.f / 255.f, 78.f / 255.f, 79.f / 255.f,
            80.f / 255.f, 81.f / 255.f, 82.f / 255.f, 83.f / 255.f, 84.f / 255.f, 85.f / 255.f, 86.f / 255.f, 87.f / 255.f, 88.f / 255.f, 89.f / 255.f,
            90.f / 255.f, 91.f / 255.f, 92.f / 255.f, 93.f / 255.f, 94.f / 255.f, 95.f / 255.f, 96.f / 255.f, 97.f / 255.f, 98.f / 255.f, 99.f / 255.f,
            100.f / 255.f, 101.f / 255.f, 102.f / 255.f, 103.f / 255.f, 104.f / 255.f, 105.f / 255.f, 106.f / 255.f, 107.f / 255.f, 108.f / 255.f, 109.f / 255.f,
            110.f / 255.f, 111.f / 255.f, 112.f / 255.f, 113.f / 255.f, 114.f / 255.f, 115.f / 255.f, 116.f / 255.f, 117.f / 255.f, 118.f / 255.f, 119.f / 255.f,
            120.f / 255.f, 121.f / 255.f, 122.f / 255.f, 123.f / 255.f, 124.f / 255.f, 125.f / 255.f, 126.f / 255.f, 127.f / 255.f, 128.f / 255.f, 129.f / 255.f,
            130.f / 255.f, 131.f / 255.f, 132.f / 255.f, 133.f / 255.f, 134.f / 255.f, 135.f / 255.f, 136.f / 255.f, 137.f / 255.f, 138.f / 255.f, 139.f / 255.f,
            140.f / 255.f, 141.f / 255.f, 142.f / 255.f, 143.f / 255.f, 144.f / 255.f, 145.f / 255.f, 146.f / 255.f, 147.f / 255.f, 148.f / 255.f, 149.f / 255.f,
            150.f / 255.f, 151.f / 255.f, 152.f / 255.f, 153.f / 255.f, 154.f / 255.f, 155.f / 255.f, 156.f / 255.f, 157.f / 255.f, 158.f / 255.f, 159.f / 255.f,
            160.f / 255.f, 161.f / 255.f, 162.f / 255.f, 163.f / 255.f, 164.f / 255.f, 165.f / 255.f, 166.f / 255.f, 167.f / 255.f, 168.f / 255.f, 169.f / 255.f,
            170.f / 255.f, 171.f / 255.f, 172.f / 255.f, 173.f / 255.f, 174.f / 255.f, 175.f / 255.f, 176.f / 255.f, 177.f / 255.f, 178.f / 255.f, 179.f / 255.f,
            180.f / 255.f, 181.f / 255.f, 182.f / 255.f, 183.f / 255.f, 184.f / 255.f, 185.f / 255.f, 186.f / 255.f, 187.f / 255.f, 188.f / 255.f, 189.f / 255.f,
            190.f / 255.f, 191.f / 255.f, 192.f / 255.f, 193.f / 255.f, 194.f / 255.f, 195.f / 255.f, 196.f / 255.f, 197.f / 255.f, 198.f / 255.f, 199.f / 255.f,
            200.f / 255.f, 201.f / 255.f, 202.f / 255.f, 203.f / 255.f, 204.f / 255.f, 205.f / 255.f, 206.f / 255.f, 207.f / 255.f, 208.f / 255.f, 209.f / 255.f,
            210.f / 255.f, 211.f / 255.f, 212.f / 255.f, 213.f / 255.f, 214.f / 255.f, 215.f / 255.f, 216.f / 255.f, 217.f / 255.f, 218.f / 255.f, 219.f / 255.f,
            220.f / 255.f, 221.f / 255.f, 222.f / 255.f, 223.f / 255.f, 224.f / 255.f, 225.f / 255.f, 226.f / 255.f, 227.f / 255.f, 228.f / 255.f, 229.f / 255.f,
            230.f / 255.f, 231.f / 255.f, 232.f / 255.f, 233.f / 255.f, 234.f / 255.f, 235.f / 255.f, 236.f / 255.f, 237.f / 255.f, 238.f / 255.f, 239.f / 255.f,
            240.f / 255.f, 241.f / 255.f, 242.f / 255.f, 243.f / 255.f, 244.f / 255.f, 245.f / 255.f, 246.f / 255.f, 247.f / 255.f, 248.f / 255.f, 249.f / 255.f,
            250.f / 255.f, 251.f / 255.f, 252.f / 255.f, 253.f / 255.f, 254.f / 255.f, 255.f / 255.f};
        //////////////////////////////////////////////////////////////////////////
        uint8_t makeColorChannelF( float _channel )
        {
            const float channel_255 = 255.5f;

            uint8_t channel8 = static_cast<uint8_t>(_channel * channel_255);

            return channel8;
        }
        //////////////////////////////////////////////////////////////////////////
        float makeColorChannel8( uint8_t _channel8 )
        {
            float channelf = one_div_255[_channel8];

            return channelf;
        }
        //////////////////////////////////////////////////////////////////////////
        ColorValue_ARGB makeRGBA8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a )
        {
#if defined(MENGINE_RENDER_COLOR_RGBA)
            uint8_t b8 = _r;
            uint8_t g8 = _g;
            uint8_t r8 = _b;
            uint8_t a8 = _a;
#else
            uint8_t r8 = _r;
            uint8_t g8 = _g;
            uint8_t b8 = _b;
            uint8_t a8 = _a;
#endif

            ColorValue_ARGB argb = MENGINE_FOURCC( b8, g8, r8, a8 );

            return argb;
        }
        //////////////////////////////////////////////////////////////////////////
        ColorValue_ARGB makeRGBAF( float _r, float _g, float _b, float _a )
        {
            uint8_t r8 = makeColorChannelF( _r );
            uint8_t g8 = makeColorChannelF( _g );
            uint8_t b8 = makeColorChannelF( _b );
            uint8_t a8 = makeColorChannelF( _a );

            ColorValue_ARGB argb = Helper::makeRGBA8( r8, g8, b8, a8 );

            return argb;
        }
        //////////////////////////////////////////////////////////////////////////
        Color makeColor8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a )
        {
            ColorValue_ARGB value = Helper::makeRGBA8( _r, _g, _b, _a );

            if( value == Detail::COLOR_IDENTITY_VALUE )
            {
                return Color();
            }

#if defined(MENGINE_RENDER_COLOR_RGBA)
            uint8_t b8 = _r;
            uint8_t g8 = _g;
            uint8_t r8 = _b;
            uint8_t a8 = _a;
#else
            uint8_t r8 = _r;
            uint8_t g8 = _g;
            uint8_t b8 = _b;
            uint8_t a8 = _a;
#endif

            float rf = one_div_255[r8];
            float gf = one_div_255[g8];
            float bf = one_div_255[b8];
            float af = one_div_255[a8];

            return Color( rf, gf, bf, af, value );
        }
        //////////////////////////////////////////////////////////////////////////
        Color makeColorF( float _r, float _g, float _b, float _a )
        {
            return Color( _r, _g, _b, _a );
        }
        //////////////////////////////////////////////////////////////////////////
        Color makeColorARGB( ColorValue_ARGB _argb )
        {
            if( _argb == Detail::COLOR_IDENTITY_VALUE )
            {
                return Color();
            }

#if defined(MENGINE_RENDER_COLOR_RGBA)
            uint8_t r8 = (_argb >> 0) & 0xFF;
            uint8_t g8 = (_argb >> 8) & 0xFF;
            uint8_t b8 = (_argb >> 16) & 0xFF;
            uint8_t a8 = (_argb >> 24) & 0xFF;
#else
            uint8_t b8 = (_argb >> 0) & 0xFF;
            uint8_t g8 = (_argb >> 8) & 0xFF;
            uint8_t r8 = (_argb >> 16) & 0xFF;
            uint8_t a8 = (_argb >> 24) & 0xFF;
#endif

            float rf = Helper::makeColorChannel8( r8 );
            float gf = Helper::makeColorChannel8( g8 );
            float bf = Helper::makeColorChannel8( b8 );
            float af = Helper::makeColorChannel8( a8 );

            return Color( rf, gf, bf, af, _argb );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

