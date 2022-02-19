#ifndef AppleSentryNative_H_
#define AppleSentryNative_H_

#include "Config/Config.h"

int appleSentryInitialize( const char * _dsn, int _debug, const char * _releaseName );
int appleSentryFinalize();

int appleSentryCapture( const char * _msg, int _code );

int appleSentrySetExtraBoolean( const char * _key, int _value );
int appleSentrySetExtraInteger( const char * _key, int _value );
int appleSentrySetExtraString( const char * _key, const char * _value );
#endif
