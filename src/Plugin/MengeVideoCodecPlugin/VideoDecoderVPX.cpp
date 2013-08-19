#	include "VideoDecoderVPX.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

#   define vpx_interface (vpx_codec_vp9_dx())

#   define IVF_FILE_HDR_SZ  (32)
#   define IVF_FRAME_HDR_SZ (12) 

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static unsigned int mem_get_le16( const void * vmem )
    {
        unsigned int  val;
        const unsigned char *mem = (const unsigned char *)vmem;

        val = mem[1] << 8;
        val |= mem[0];

        return val;
    }
    //////////////////////////////////////////////////////////////////////////    
    static unsigned int mem_get_le32( const void * vmem )
    {
        unsigned int  val;
        const unsigned char *mem = (const unsigned char *)vmem;

        val = mem[3] << 24;
        val |= mem[2] << 16;
        val |= mem[1] << 8;
        val |= mem[0];

        return val;
    }
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderVPX::VideoDecoderVPX()
        : m_frame(nullptr)
        , m_iterator(nullptr)
        , m_pitch(0)
        , m_pts(0.f)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderVPX::~VideoDecoderVPX()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::_initialize()
	{        
        unsigned char file_hdr[IVF_FILE_HDR_SZ];

        m_stream->read( file_hdr, IVF_FILE_HDR_SZ );

        if( file_hdr[0] != 'D' || file_hdr[1] != 'K' || file_hdr[2] != 'I' || file_hdr[3] !='F' )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderVPX::_initialize is not an IVF file"
                );

            return false;
        }

        unsigned int fourcc = mem_get_le32( file_hdr + 8 );
        unsigned int width = mem_get_le16( file_hdr + 12 );
        unsigned int height = mem_get_le16( file_hdr + 14 );
        unsigned int fps_num = mem_get_le32( file_hdr + 16 );
        unsigned int fps_den = mem_get_le32( file_hdr + 20 );

        int flags = 0;
        vpx_codec_err_t err_dec_init = vpx_codec_dec_init( &m_codec, vpx_interface, NULL, flags);

        if( err_dec_init != VPX_CODEC_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderVPX::_initialize vpx_codec_dec_init %d"
                , err_dec_init
                );
            
            return false;
        }

        m_dataInfo.frameWidth = width;
        m_dataInfo.frameHeight = height;
        m_dataInfo.frameTiming = 33.3333f;
        m_dataInfo.fps = 30;

        return true;
	}

#define YG 74 /* static_cast<int8>(1.164 * 64 + 0.5) */

#define UB 127 /* min(63,static_cast<int8>(2.018 * 64)) */
#define UG -25 /* static_cast<int8>(-0.391 * 64 - 0.5) */
#define UR 0

#define VB 0
#define VG -52 /* static_cast<int8>(-0.813 * 64 - 0.5) */
#define VR 102 /* static_cast<int8>(1.596 * 64 + 0.5) */

#define BB UB * 128 + VB * 128
#define BG UG * 128 + VG * 128
#define BR UR * 128 + VR * 128
    //////////////////////////////////////////////////////////////////////////
    inline static unsigned char Clip(int val) 
    {
        if( val < 0 )
        {
            return static_cast<unsigned char>(0);
        } 
        else if (val > 255) 
        {
            return static_cast<unsigned char>(255);
        }

        return static_cast<unsigned char>(val);
    }
    //////////////////////////////////////////////////////////////////////////
    inline static void YuvPixel(unsigned char y, unsigned char u, unsigned char v, unsigned char* rgb_buf)
    {
        int y1 = (static_cast<int>(y) - 16) * YG;
        rgb_buf[0] = Clip(static_cast<int>((u * UB + v * VB) - (BB) + y1) >> 6);
        rgb_buf[1] = Clip(static_cast<int>((u * UG + v * VG) - (BG) + y1) >> 6);
        rgb_buf[2] = Clip(static_cast<int>((u * UR + v * VR) - (BR) + y1) >> 6);
    }
    //////////////////////////////////////////////////////////////////////////
    inline static void I422ToARGBRow(const unsigned char* y_buf, const unsigned char* u_buf,
        const unsigned char* v_buf, unsigned char* rgb_buf, int width)
    {
        for (int x = 0; x < width - 1; x += 2) {
            YuvPixel(y_buf[0], u_buf[0], v_buf[0], rgb_buf + 0);
            YuvPixel(y_buf[1], u_buf[0], v_buf[0], rgb_buf + 4);
            y_buf += 2;
            u_buf += 1;
            v_buf += 1;
            rgb_buf += 8;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    inline static void I420ToRGB24(
        const unsigned char* src_y, int src_stride_y,
        const unsigned char* src_u, int src_stride_u,
        const unsigned char* src_v, int src_stride_v,
        unsigned char* dst_rgb, int dst_stride_rgb,
        int width, int height)
    {
        for (int y = 0; y < height; ++y) {
            I422ToARGBRow(src_y, src_u, src_v, dst_rgb, width);
            dst_rgb += dst_stride_rgb;
            src_y += src_stride_y;
            if (y & 1) {
                src_u += src_stride_u;
                src_v += src_stride_v;

            }
        }
    }
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderVPX::decode( void * _buffer, size_t _bufferSize )
	{
        vpx_codec_iter_t iter = nullptr;
        vpx_image_t * img = vpx_codec_get_frame( &m_codec, &iter );

        if( img == nullptr )
        {
            return 0;
        }

        unsigned char * vpx_buff = static_cast<unsigned char *>(_buffer);

        I420ToRGB24(
            img->planes[0], img->stride[0],
            img->planes[1], img->stride[1],
            img->planes[2], img->stride[2],
            vpx_buff, m_pitch,
            img->d_w, img->d_h);

        //unsigned char * Ys = img->planes[0];
        //unsigned char * Us = img->planes[1];
        //unsigned char * Vs = img->planes[2];
        //    
        //unsigned char * vpx_buff = static_cast<unsigned char *>(_buffer);
        //for( size_t j = 0; j != img->d_h; ++j )
        //{
        //    unsigned char * yy = Ys;
        //    unsigned char * uu = Us;
        //    unsigned char * vv = Vs;

        //    unsigned char * vpx_buff_rgb = vpx_buff;

        //    for( size_t i = 0; i < img->d_w - 1; i += 2 )
        //    {
        //        YuvPixel( yy[0], uu[0], vv[0], vpx_buff + (i + 0) * 4 );
        //        YuvPixel( yy[1], uu[0], vv[0], vpx_buff + (i + 1) * 4 );

        //        yy += 2;
        //        uu += 1;
        //        vv += 1;

        //        vpx_buff_rgb += 8;
        //    }

        //    Ys += img->stride[0];

        //    if( j & 1 )
        //    {
        //        Us += img->stride[1];
        //        Vs += img->stride[2];
        //    }

        //    vpx_buff += m_pitch;
        //}

        return m_frameSize;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderVPX::readNextFrame( float & _pts )
	{	
        unsigned char frame_hdr[IVF_FRAME_HDR_SZ];

        size_t frame_hdr_read_size = m_stream->read( frame_hdr, IVF_FRAME_HDR_SZ );

        if( frame_hdr_read_size != IVF_FRAME_HDR_SZ )
        {
            return VDRS_END_STREAM;
        }

        int frame_sz = mem_get_le32( frame_hdr );
        
        delete [] m_frame;
        m_frame = new unsigned char [frame_sz];
                        
        size_t frame_read_size = m_stream->read( m_frame, frame_sz );
        
        if( frame_read_size != frame_sz)
        {            
            return VDRS_FAILURE;
        }

        m_frameSize = frame_sz;

        vpx_codec_err_t err_decode = vpx_codec_decode( &m_codec, m_frame, m_frameSize, NULL, 0);

        if( err_decode != VPX_CODEC_OK )
        {
            const char * err_decode_detail = vpx_codec_error_detail( &m_codec );

            LOGGER_ERROR(m_serviceProvider)("VideoDecoderVPX::decode vpx_codec_decode %d %s"
                , err_decode
                , err_decode_detail
                );

            return VDRS_FAILURE;
        }
        
        m_pts += m_dataInfo.frameTiming;

        return VDRS_SUCCESS;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::seek( float _timing )
	{
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderVPX::setPitch( size_t _pitch )
    {
        m_pitch = _pitch;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::eof()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderVPX::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}