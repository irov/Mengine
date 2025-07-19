#include "AppleGameCenterScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"


#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonTraceback.h"
#import "Environment/Python/ApplePythonProvider.h"

#import "AppleGameCenterApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

@interface PythonAppleGameCenterConnectCallback : ApplePythonProvider<AppleGameCenterConnectCallbackInterface>
@end

@implementation PythonAppleGameCenterConnectCallback

- (void)onAppleGameCenterAuthenticate:(BOOL)successful {
    pybind::object py_cb = [self getMethod:@"onAppleGameCenterAuthenticate"];
        
    if( py_cb.is_callable() == false )
    {
        return;
    }
        
    py_cb.call_args( successful == YES, self.m_args );
}

- (void)onAppleGameCenterSynchronizate:(BOOL)successful {
    pybind::object py_cb = [self getMethod:@"onAppleGameCenterSynchronizate"];
        
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
        static void AppleGameCenter_connect( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleGameCenterConnectCallbackInterface> callback
                = [[PythonAppleGameCenterConnectCallback alloc] initWithCbs:_cbs args:_args];
            
            [[AppleGameCenterApplicationDelegate sharedInstance] connect:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_isConnect()
        {
            BOOL successful = [[AppleGameCenterApplicationDelegate sharedInstance] isConnect];
            
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_reportAchievement( NSString * _identifier, double _percent, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;
            
            BOOL result = [[AppleGameCenterApplicationDelegate sharedInstance] reportAchievement:_identifier
                                                                                         percent:_percent
                                                                                        response:^(BOOL successful) {
                copy_cb.call_args( successful == YES, copy_args );
            }];

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_checkAchievement( NSString * _identifier )
        {
            BOOL successful = [[AppleGameCenterApplicationDelegate sharedInstance] checkAchievement:_identifier];
            
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_resetAchievements( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;
            
            BOOL result = [[AppleGameCenterApplicationDelegate sharedInstance] resetAchievements:^(NSError * error) {
                if (error != nil) {
                    copy_cb.call_args( false, copy_args );
                } else {
                    copy_cb.call_args( true, copy_args );
                }
            }];

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_reportScore( NSString * _identifier, int64_t _score, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;
            
            BOOL result = [[AppleGameCenterApplicationDelegate sharedInstance] reportScore:_identifier score:_score response:^(NSError * error) {
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
    AppleGameCenterScriptEmbedding::AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterScriptEmbedding::~AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "appleGameCenterConnect", &Detail::AppleGameCenter_connect );
        pybind::def_function( _kernel, "appleGameCenterIsConnect", &Detail::AppleGameCenter_isConnect );
        pybind::def_function_args( _kernel, "appleGameCenterReportAchievement", &Detail::AppleGameCenter_reportAchievement );
        pybind::def_function( _kernel, "appleGameCenterCheckAchievement", &Detail::AppleGameCenter_checkAchievement );
        pybind::def_function_args( _kernel, "appleGameCenterResetAchievements", &Detail::AppleGameCenter_resetAchievements );
        pybind::def_function_args( _kernel, "appleGameCenterReportScore", &Detail::AppleGameCenter_reportScore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
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

