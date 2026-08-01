#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class MCPInputContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPInputKeyboardCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPInputKeyboardCommand( MCPInputContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        typedef bool (MCPInputKeyboardCommand::*Action)( const jpp::object & _params, MCPCommandResponse * const _response );
        typedef Map<ConstString, Action> MapActions;

        bool text_( const jpp::object & _params, MCPCommandResponse * const _response );
        bool down_( const jpp::object & _params, MCPCommandResponse * const _response );
        bool up_( const jpp::object & _params, MCPCommandResponse * const _response );
        bool press_( const jpp::object & _params, MCPCommandResponse * const _response );
        bool pushKey_( const jpp::object & _params, bool _down, MCPCommandResponse * const _response );

    protected:
        MCPInputContext * m_context;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputKeyboardCommand, MCPCommandInterface> MCPInputKeyboardCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
