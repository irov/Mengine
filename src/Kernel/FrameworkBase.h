#pragma once

#include "Interface/FrameworkInterface.h"

#include "Kernel/Observable.h"

namespace Mengine
{
    class FrameworkBase
        : public FrameworkInterface
        , public Observable
    {
    public:
        FrameworkBase();
        ~FrameworkBase() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        bool initializeFramework() override;
        void finalizeFramework() override;

    public:
        bool runFramework() override;
        void stopFramework() override;

    public:
        bool isAvailableFramework() const override;

    protected:
        virtual bool _initializeFramework();
        virtual void _finalizeFramework();

        virtual bool _runFramework();
        virtual void _stopFramework();

    protected:
        virtual bool _availableFramework() const;

    protected:
        ConstString m_name;

        bool m_available;
        bool m_initialize;
        bool m_running;
    };
}