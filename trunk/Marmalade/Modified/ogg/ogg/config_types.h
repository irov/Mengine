#ifndef __CONFIG_TYPES_H__
#define __CONFIG_TYPES_H__

#ifdef _WIN32
* MSVC/Borland */
	typedef __int64 ogg_int64_t;
	typedef __int32 ogg_int32_t;
	typedef unsigned __int32 ogg_uint32_t;
	typedef __int16 ogg_int16_t;
	typedef unsigned __int16 ogg_uint16_t;
#else
	 typedef short ogg_int16_t;
	 typedef unsigned short ogg_uint16_t;
	 typedef int ogg_int32_t;
	 typedef unsigned int ogg_uint32_t;
	 typedef long long ogg_int64_t;
#endif
#endif  /* _OS_TYPES_H */
