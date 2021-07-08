#pragma once

#include "Config/Char.h"

#include <exception>

#ifndef MENGINE_EXCEPTION_MAX_MESSAGE
#define MENGINE_EXCEPTION_MAX_MESSAGE 4096
#endif

namespace Mengine
{
    class Exception
        : public std::exception
    {
    public:
        explicit Exception( const Char * _message ) noexcept;
        ~Exception() noexcept;

    public:
        const char * what() const noexcept override;

    protected:
        Char m_message[MENGINE_EXCEPTION_MAX_MESSAGE] = {'\0'};
    };
}