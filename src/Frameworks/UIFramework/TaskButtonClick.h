#pragma once

#include "Button.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskButtonClick
        : public GOAP::Task
    {
    public:
        explicit TaskButtonClick( const ButtonPtr & _button );
        ~TaskButtonClick() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        ButtonPtr m_button;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskButtonClick> TaskButtonClickPtr;
    //////////////////////////////////////////////////////////////////////////
}