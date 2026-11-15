#pragma once

#include "OpButton/OpButtonLoggerListener.h"

namespace Mengine
{
    class Win32OpButtonLoggerListener
        : public OpButton::OpButtonLoggerListener
    {
    public:
        Win32OpButtonLoggerListener();
        ~Win32OpButtonLoggerListener() override;

    public:
        void onOpButtonLoggerMessage( OpButton::OpButtonLogLevel _level, const OpButton::OpButtonString & _tag, const OpButton::OpButtonString & _message ) override;
    };
}
