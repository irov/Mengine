#include "GlobalInputHandlerHelper.h"

#include "Interface/GlobalInputHandlerInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        namespace
        {
            //////////////////////////////////////////////////////////////////////////
            class GlobalKeyHandler
                : public InputHandlerInterface
                , public Factorable
            {
            public:
                GlobalKeyHandler( KeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event )
                    : m_code( _code )
                    , m_isDown( _isDown )
                    , m_event( _event )
                {
                }

                ~GlobalKeyHandler() override
                {
                }

            protected:
                bool handleKeyEvent( const InputKeyEvent & _event ) override
                {
                    if( m_code != _event.code || m_isDown != _event.isDown )
                    {
                        return false;
                    }

                    m_event( _event );

                    return false;
                }

            protected:
                bool handleTextEvent( const InputTextEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

            protected:
                bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                bool handleMouseMove( const InputMouseMoveEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                bool handleMouseEnter( const InputMouseEnterEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    return false;
                }

                void handleMouseLeave( const InputMouseLeaveEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty
                }

            protected:
                KeyCode m_code;
                bool m_isDown;

                LambdaInputKeyEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<GlobalKeyHandler> GlobalKeyHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalKeyHandler( KeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const Char * _doc )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            GlobalKeyHandlerPtr handler = Helper::makeFactorableUnique<GlobalKeyHandler>( _code, _isDown, _event );

            uint32_t id = globalInputHandle->addGlobalHandler( handler, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void removeGlobalHandler( uint32_t _id )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            globalInputHandle->removeGlobalHandler( _id );
        }
    }
}