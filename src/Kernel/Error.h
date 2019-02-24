#pragma once

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ErrorOperator
    {
    public:
        ErrorOperator( const Char * _file, uint32_t _line );
        ~ErrorOperator();

    public:
        void operator()( const Char * _format, ... ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#ifdef __GNUC__
//////////////////////////////////////////////////////////////////////////
#define ERROR_FATAL\
	Mengine::ErrorOperator( __PRETTY_FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define ERROR_FATAL\
	Mengine::ErrorOperator( __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#endif