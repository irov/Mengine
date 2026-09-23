#include "iOSNativePythonScriptEmbedding.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/Python/PythonScriptWrapper.h"
#import "Environment/Python/PythonCallbackProvider.h"

#import "iOSNativePythonPlugin.h"

#include "iOSNativePythonTypeCast.h"

#include "Kernel/ThreadHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSString_type
            : public pybind::type_cast_result<NSString *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                if( _kernel->string_check( _obj ) == true )
                {
                    const Char * value_char = _kernel->string_to_char( _obj );

                    if( value_char == nullptr )
                    {
                        return false;
                    }

                    NSString * value = [NSString stringWithUTF8String:value_char];

                    if( value == nil )
                    {
                        return false;
                    }

                    _value = value;
                }
                else
                {
                    return false;
                }

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                if( _value == nil )
                {
                    return _kernel->ret_none();
                }

                const Char * value_str = [_value UTF8String];

                PyObject * py_value = _kernel->string_from_char( value_str );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSDictionary_type
            : public pybind::type_cast_result<NSDictionary *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                if( _kernel->dict_check( _obj ) == false )
                {
                    return false;
                }

                id value = pybind::extract<id>( _kernel, _obj );

                _value = value;

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                id value = _value;

                PyObject * py_value = pybind::ptr_throw( _kernel, value );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSSet_type
            : public pybind::type_cast_result<NSSet *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                NSMutableSet * set = [NSMutableSet set];

                PyObject * py_iterator;
                if( _kernel->iterator_get( _obj, &py_iterator ) == false )
                {
                    return false;
                }

                pybind::object iterator_owner( _kernel, py_iterator, pybind::borrowed );

                PyObject * py_value;
                while( _kernel->iterator_next( py_iterator, &py_value ) == true )
                {
                    pybind::object value_owner( _kernel, py_value, pybind::borrowed );

                    id value = pybind::extract<id>( _kernel, py_value );

                    [set addObject:value];
                }

                _value = set;

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                id value = _value;

                PyObject * py_value = pybind::ptr_throw( _kernel, value );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<class PythonAppleSemaphoreListener, AppleSemaphoreListenerInterface> PythonAppleSemaphoreListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleSemaphoreListener
            : public AppleSemaphoreListenerInterface
            , public PythonCallbackProvider
        {
            DECLARE_FACTORABLE("PythonAppleSemaphoreListener")

        public:
            PythonAppleSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
                : PythonCallbackProvider( _cb, _args )
            {
            }

        protected:
            void invoke( id _value ) override
            {
                PythonAppleSemaphoreListenerPtr keep = PythonAppleSemaphoreListenerPtr::from( this );

                Helper::dispatchMainThreadEvent( [keep, _value]() {
                    keep->call_cb( _value );
                } );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void iOSNativePython_showToast( NSString * _message )
        {
            [iOSDetail showToast:_message];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSNativePython_activateSemaphore( NSString * _name, id _value )
        {
            MENGINE_ASSERTION_FATAL( [_value isKindOfClass:[NSNull class]] == NO, "activate semaphore '%s' invalid value None"
                , [_name UTF8String]
            );

            [[iOSNativePythonPlugin sharedInstance] activateSemaphore:_name withValue:_value];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSNativePython_waitSemaphore( NSString * _name, const pybind::object & _cb, const pybind::args & _args )
        {
            AppleSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAppleSemaphoreListener>( MENGINE_DOCUMENT_PYTHON, _cb, _args );

            [[iOSNativePythonPlugin sharedInstance] waitSemaphore:_name listener:listener];
        }
        ///////////////////////////////////////////////////////////////////////
        static void iOSNativePython_showAreYouSureAlertDialog( NSString * _title, NSString * _message, double _delay, const pybind::object & _yesCb, const pybind::object & _cancelCb, const pybind::args & _args )
        {
            pybind::object yesCb = _yesCb;
            pybind::object cancelCb = _cancelCb;
            pybind::args args = _args;

            [iOSDetail showAreYouSureAlertDialogWithTitle:_title
                                                  message:_message
                                                    delay:_delay
                                                      yes:^() {
                yesCb.call_args( args );
            }
                                                   cancel:^() {
                cancelCb.call_args( args );
            }];
        }
        ///////////////////////////////////////////////////////////////////////
        static void iOSNativePython_showOkAlert( NSString * _title, NSString * _message, const pybind::object & _okCb, const pybind::args & _args )
        {
            pybind::object okCb = _okCb;
            pybind::args args = _args;

            [iOSDetail showOkAlertWithTitle:_title
                                    message:_message
                                         ok:^() {
                okCb.call_args( args );
            }];
        }
        ///////////////////////////////////////////////////////////////////////
        static void iOSNativePython_setIdleTimerDisabled( bool _disabled )
        {
            UIApplication.sharedApplication.idleTimerDisabled = _disabled;
        }
        ///////////////////////////////////////////////////////////////////////
        static NSString * iOSNativePython_getUserId()
        {
            iOSApplication * application = [iOSApplication sharedInstance];
            NSString * userId = [application getUserId];

            return userId;
        }
        ///////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSNativePythonScriptEmbedding::iOSNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSNativePythonScriptEmbedding::~iOSNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSNativePythonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::registration_type_cast<NSString *>(_kernel, pybind::make_type_cast<Detail::extract_NSString_type>(_kernel));
        pybind::registration_type_cast<NSDictionary *>(_kernel, pybind::make_type_cast<Detail::extract_NSDictionary_type>(_kernel));
        pybind::registration_type_cast<NSSet *>(_kernel, pybind::make_type_cast<Detail::extract_NSSet_type>(_kernel));

        pybind::def_function( _kernel, "activateSemaphore", &Detail::iOSNativePython_activateSemaphore );
        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::iOSNativePython_waitSemaphore );
        pybind::def_function_args( _kernel, "iOSNativePythonShowAreYouSureAlertDialog", &Detail::iOSNativePython_showAreYouSureAlertDialog );
        pybind::def_function_args( _kernel, "iOSNativePythonShowOkAlert", &Detail::iOSNativePython_showOkAlert );
        pybind::def_function( _kernel, "iOSNativePythonSetIdleTimerDisabled", &Detail::iOSNativePython_setIdleTimerDisabled );
        pybind::def_function( _kernel, "iOSNativePythonGetUserId", &Detail::iOSNativePython_getUserId );
        pybind::def_function( _kernel, "iOSNativePythonShowToast", &Detail::iOSNativePython_showToast );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "activateSemaphore", nullptr );
        _kernel->remove_from_module( "waitSemaphore", nullptr );

        _kernel->remove_from_module( "iOSNativePythonGetUserId", nullptr );
        _kernel->remove_from_module( "iOSNativePythonShowToast", nullptr );

        pybind::unregistration_type_cast<NSString *>( _kernel );
        pybind::unregistration_type_cast<NSDictionary *>( _kernel );
        pybind::unregistration_type_cast<NSSet *>( _kernel );
    }
    //////////////////////////////////////////////////////////////////////////
}
