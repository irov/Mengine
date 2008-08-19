#pragma once

#	include <fstream>
#	include "vorbis/vorbisfile.h"

size_t s_readOgg ( void *_ptr, size_t _size, size_t _nmemb, void *_datasource );
int    s_seekOgg ( void *_datasource, ogg_int64_t _offset, int _whence );
long   s_tellOgg ( void *_datasource );
int    s_closeOgg( void *_datasource );

size_t s_readOgg_ ( void *_ptr, size_t _size, size_t _nmemb, void *_datasource );
int    s_seekOgg_ ( void *_datasource, ogg_int64_t _offset, int _whence );
long   s_tellOgg_ ( void *_datasource );
int    s_closeOgg_( void *_datasource );

// For mstream
/*size_t ReadOgg2 ( void *_ptr, size_t _size, size_t _nmemb, void *_datasource );
int    SeekOgg2 ( void *_datasource, ogg_int64_t _offset, int _whence );
long   TellOgg2 ( void *_datasource );
int    CloseOgg2( void *_datasource );*/