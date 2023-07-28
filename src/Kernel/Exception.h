#pragma once

#include "Kernel/StaticString.h"

#include <exception>

#ifndef MENGINE_EXCEPTION_MAX_MESSAGE
#define MENGINE_EXCEPTION_MAX_MESSAGE 8192
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
        StaticString<MENGINE_EXCEPTION_MAX_MESSAGE> m_message;
    };
}