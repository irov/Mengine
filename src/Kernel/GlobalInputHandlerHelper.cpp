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
            class GlobalBaseHandler
                : public InputHandlerInterface
                , public Factorable
            {
            public:
                GlobalBaseHandler()
                {
                }

                ~GlobalBaseHandler() override
                {
                }

            protected:
                bool handleKeyEvent( const InputKeyEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

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
            };
            //////////////////////////////////////////////////////////////////////////
            class GlobalKeyHandler
                : public GlobalBaseHandler
            {
            public:
                GlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event )
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
                EKeyCode m_code;
                bool m_isDown;

                LambdaInputKeyEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<GlobalKeyHandler> GlobalKeyHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            class GlobalMouseButtonHandler
                : public GlobalBaseHandler
            {
            public:
                GlobalMouseButtonHandler( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _event )
                    : m_code( _code )
                    , m_isDown( _isDown )
                    , m_event( _event )
                {
                }

                ~GlobalMouseButtonHandler() override
                {
                }

            protected:
                bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
                {
                    if( m_code != _event.code || m_isDown != _event.isDown )
                    {
                        return false;
                    }

                    m_event( _event );

                    return false;
                }

            protected:
                EMouseCode m_code;
                bool m_isDown;

                LambdaInputMouseButtonEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<GlobalMouseButtonHandler> GlobalMouseButtonHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            class GlobalMouseMoveHandler
                : public GlobalBaseHandler
            {
            public:
                GlobalMouseMoveHandler( const LambdaInputMouseMoveEvent & _event )
                    : m_event( _event )
                {
                }

                ~GlobalMouseMoveHandler() override
                {
                }

            protected:
                bool handleMouseMove( const InputMouseMoveEvent & _event ) override
                {
                    m_event( _event );

                    return false;
                }

            protected:
                LambdaInputMouseMoveEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<GlobalMouseMoveHandler> GlobalMouseMoveHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            class GlobalMouseWheelHandler
                : public GlobalBaseHandler
            {
            public:
                GlobalMouseWheelHandler( const LambdaInputMouseWheelEvent & _event )
                    : m_event( _event )
                {
                }

                ~GlobalMouseWheelHandler() override
                {
                }

            protected:
                bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
                {
                    m_event( _event );

                    return false;
                }

            protected:
                LambdaInputMouseWheelEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<GlobalMouseMoveHandler> GlobalMouseMoveHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const Char * _doc )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            InputHandlerInterfacePtr handler = Helper::makeFactorableUnique<GlobalKeyHandler>( _code, _isDown, _event );

            uint32_t id = globalInputHandle->addGlobalHandler( handler, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalMouseButtonEvent( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _event, const Char * _doc )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            InputHandlerInterfacePtr handler = Helper::makeFactorableUnique<GlobalMouseButtonHandler>( _code, _isDown, _event );

            uint32_t id = globalInputHandle->addGlobalHandler( handler, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalMouseMoveEvent( const LambdaInputMouseMoveEvent & _event, const Char * _doc )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            InputHandlerInterfacePtr handler = Helper::makeFactorableUnique<GlobalMouseMoveHandler>( _event );

            uint32_t id = globalInputHandle->addGlobalHandler( handler, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalMouseWheelEvent( const LambdaInputMouseWheelEvent & _event, const Char * _doc )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandle = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            InputHandlerInterfacePtr handler = Helper::makeFactorableUnique<GlobalMouseWheelHandler>( _event );

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