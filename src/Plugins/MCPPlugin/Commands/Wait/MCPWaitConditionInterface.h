#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/Map.h"
#include "Kernel/Mixin.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MCPWaitConditionRequest
    {
        const jpp::object & condition;
        uint64_t startGeneration;
        uint64_t currentGeneration;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MCPWaitConditionResponse
    {
        MCPWaitConditionResponse();

        bool satisfied;
        jpp::object details;
        String errorCode;
        String errorMessage;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPWaitConditionInterface
        : public Mixin
    {
    public:
        virtual bool evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPWaitConditionInterface> MCPWaitConditionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MCPWaitConditionRegistry
    {
    public:
        bool addCondition( const ConstString & _name, const MCPWaitConditionInterfacePtr & _condition );
        MCPWaitConditionInterfacePtr findCondition( const ConstString & _name ) const;
        void clear();

    protected:
        typedef Map<ConstString, MCPWaitConditionInterfacePtr> MapConditions;

        MapConditions m_conditions;
    };
    //////////////////////////////////////////////////////////////////////////
}
