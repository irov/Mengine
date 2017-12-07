#   pragma once

#   include "GOAP/GOAP.h"

#   include "pybind/object.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class PybindTaskGenerator;
    //////////////////////////////////////////////////////////////////////////
    class PybindTask
        : public GOAP::Task
    {
    public:
        PybindTask();
        ~PybindTask();

    public:
        void setGenerator( PybindTaskGenerator * _generator );
        PybindTaskGenerator * getGenerator() const;

    public:
        void setScriptObject( const pybind::object & _object );
        const pybind::object & getScriptObject() const;

    protected:
        bool _onInitialize() override;
        void _onFinalize() override;

    protected:
        bool _onValidate() const override;
        bool _onCheck() override;
        bool _onRun() override;
        bool _onSkipable() const override;
        void _onSkipNoSkiped() override;
        bool _onSkipBlock() override;
        void _onComplete() override;
        void _onSkip() override;
        void _onCancel() override;
        void _onFinally() override;
        bool _onCheckRun() const override;
        bool _onCheckSkip() const override;

    protected:
        PybindTaskGenerator * m_generator;

        pybind::object m_object;
    };

    typedef GOAP::IntrusivePtr<PybindTask> PybindTaskPtr;
}