#pragma once

#include "Interface/ScriptWrapperInterface.h"

#include "Kernel/FactoryPrototypeGenerator.h"
#include "Kernel/Observable.h"
#include "Kernel/Scriptable.h"

namespace Mengine
{
    class BaseScriptablePrototypeGenerator
        : public FactoryPrototypeGenerator
        , public Observable
    {
    public:
        BaseScriptablePrototypeGenerator();
        ~BaseScriptablePrototypeGenerator() override;

    protected:
        const ScriptWrapperInterfacePtr & getScriptWrapper() const;

    protected:
        void registerScriptWrapperObserver();
        void unregisterScriptWrapperObserver();

    protected:
        void setupScriptable( const ScriptablePtr & _scriptable );

    protected:
        ScriptWrapperInterfacePtr m_scriptWrapper;
    };
}