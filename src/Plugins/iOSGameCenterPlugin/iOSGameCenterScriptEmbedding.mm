#include "iOSGameCenterScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"


#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonTraceback.h"
#import "Environment/Python/ApplePythonProvider.h"

#import "iOSGameCenterPlugin.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

@interface PythoniOSGameCenterConnectCallback : ApplePythonProvider<iOSGameCenterConnectCallbackInterface>
@end

@implementation PythoniOSGameCenterConnectCallback

- (void)oniOSGameCenterAuthenticate:(BOOL)successful {
    pybind::object py_cb = [self getMethod:@"oniOSGameCenterAuthenticate"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( successful == YES, self.m_args );
}

- (void)oniOSGameCenterSynchronize:(BOOL)successful {
    pybind::object py_cb = [self getMethod:@"oniOSGameCenterSynchronize"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( successful == YES, self.m_args );
}

@end

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSGameCenter_connect( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSGameCenterConnectCallbackInterface> callback
                = [[PythoniOSGameCenterConnectCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSGameCenterPlugin sharedInstance] connect:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSGameCenter_isConnect()
        {
            BOOL successful = [[iOSGameCenterPlugin sharedInstance] isConnect];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSGameCenter_reportAchievement( NSString * _identifier, double _percent, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            BOOL result = [[iOSGameCenterPlugin sharedInstance] reportAchievement:_identifier
                                                                                         percent:_percent
                                                                                        response:^(BOOL successful) {
                copy_cb.call_args( successful == YES, copy_args );
            }];

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSGameCenter_checkAchievement( NSString * _identifier )
        {
            BOOL successful = [[iOSGameCenterPlugin sharedInstance] checkAchievement:_identifier];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSGameCenter_resetAchievements( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            BOOL result = [[iOSGameCenterPlugin sharedInstance] resetAchievements:^(NSError * error) {
                if (error != nil) {
                    copy_cb.call_args( false, copy_args );
                } else {
                    copy_cb.call_args( true, copy_args );
                }
            }];

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSGameCenter_reportScore( NSString * _identifier, int64_t _score, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            BOOL result = [[iOSGameCenterPlugin sharedInstance] reportScore:_identifier score:_score response:^(NSError * error) {
                if (error != nil) {
                    copy_cb.call_args( false, copy_args );
                } else {
                    copy_cb.call_args( true, copy_args );
                }
            }];

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSGameCenterScriptEmbedding::iOSGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSGameCenterScriptEmbedding::~iOSGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSGameCenterScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "appleGameCenterConnect", &Detail::iOSGameCenter_connect );
        pybind::def_function( _kernel, "appleGameCenterIsConnect", &Detail::iOSGameCenter_isConnect );
        pybind::def_function_args( _kernel, "appleGameCenterReportAchievement", &Detail::iOSGameCenter_reportAchievement );
        pybind::def_function( _kernel, "appleGameCenterCheckAchievement", &Detail::iOSGameCenter_checkAchievement );
        pybind::def_function_args( _kernel, "appleGameCenterResetAchievements", &Detail::iOSGameCenter_resetAchievements );
        pybind::def_function_args( _kernel, "appleGameCenterReportScore", &Detail::iOSGameCenter_reportScore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleGameCenterConnect", nullptr );
        _kernel->remove_from_module( "appleGameCenterIsConnect", nullptr );
        _kernel->remove_from_module( "appleGameCenterReportAchievement", nullptr );
        _kernel->remove_from_module( "appleGameCenterCheckAchievement", nullptr );
        _kernel->remove_from_module( "appleGameCenterResetAchievements", nullptr );
        _kernel->remove_from_module( "appleGameCenterReportScore", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
