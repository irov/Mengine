#	pragma once

namespace Menge
{
	namespace Utils
	{
		void I420ToRGB24(
			const unsigned char* src_y, int src_stride_y,
			const unsigned char* src_u, int src_stride_u,
			const unsigned char* src_v, int src_stride_v,
			unsigned char* dst_rgb, int dst_stride_rgb,
			int width, int height );
	}
}	// namespace Menge
