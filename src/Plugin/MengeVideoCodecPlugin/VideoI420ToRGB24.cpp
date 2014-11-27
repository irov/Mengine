#	include "VideoI420ToRGB24.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const int YG = 74; /* static_cast<int8>(1.164 * 64 + 0.5) */

	static const int UB = 127; /* min(63,static_cast<int8>(2.018 * 64)) */
	static const int UG = -25; /* static_cast<int8>(-0.391 * 64 - 0.5) */
	static const int UR = 0;

	static const int VB = 0;
	static const int VG = -52; /* static_cast<int8>(-0.813 * 64 - 0.5) */
	static const int VR = 102; /* static_cast<int8>(1.596 * 64 + 0.5) */
	
	static const int BB = UB * 128 + VB * 128;
	static const int BG = UG * 128 + VG * 128;
	static const int BR = UR * 128 + VR * 128;
	//////////////////////////////////////////////////////////////////////////
	inline static unsigned char s_clip( int val ) 
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
	inline static void s_yuvPixel(unsigned char y, unsigned char u, unsigned char v, unsigned char* rgb_buf)
	{
		int y1 = (static_cast<int>(y) - 16) * YG;
		rgb_buf[0] = s_clip(static_cast<int>((u * UB + v * VB) - (BB) + y1) >> 6);
		rgb_buf[1] = s_clip(static_cast<int>((u * UG + v * VG) - (BG) + y1) >> 6);
		rgb_buf[2] = s_clip(static_cast<int>((u * UR + v * VR) - (BR) + y1) >> 6);
	}
	//////////////////////////////////////////////////////////////////////////
	inline static void s_I422ToARGBRow(const unsigned char* y_buf, const unsigned char* u_buf,
		const unsigned char* v_buf, unsigned char* rgb_buf, int width)
	{
		for (int x = 0; x < width - 1; x += 2) {
			s_yuvPixel(y_buf[0], u_buf[0], v_buf[0], rgb_buf + 0);
			s_yuvPixel(y_buf[1], u_buf[0], v_buf[0], rgb_buf + 4);
			y_buf += 2;
			u_buf += 1;
			v_buf += 1;
			rgb_buf += 8;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Utils
	{
		void I420ToRGB24(
			const unsigned char* src_y, int src_stride_y,
			const unsigned char* src_u, int src_stride_u,
			const unsigned char* src_v, int src_stride_v,
			unsigned char* dst_rgb, int dst_stride_rgb,
			int width, int height)
		{
			for (int y = 0; y < height; ++y) 
			{
				s_I422ToARGBRow(src_y, src_u, src_v, dst_rgb, width);

				dst_rgb += dst_stride_rgb;
				src_y += src_stride_y;
				if (y & 1) 
				{
					src_u += src_stride_u;
					src_v += src_stride_v;
				}
			}
		}
	}
}	// namespace Menge
