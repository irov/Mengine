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
        static bool iOSGameCenter_requestIdentityVerificationSignature( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            BOOL result = [[iOSGameCenterPlugin sharedInstance] requestIdentityVerificationSignature:^(NSError * error, NSString * playerId, NSURL * publicKeyURL, NSData * signature, NSData * salt, uint64_t timestamp) {
                if (error != nil || playerId == nil || publicKeyURL == nil || signature == nil || salt == nil) {
                    copy_cb.call_args( false, @"", @"", @"", @"", @"", copy_args );

                    return;
                }

                NSString * signatureBase64 = [signature base64EncodedStringWithOptions:0];
                NSString * saltBase64 = [salt base64EncodedStringWithOptions:0];
                NSString * timestampString = [NSString stringWithFormat:@"%llu", (unsigned long long)timestamp];

                copy_cb.call_args( true, playerId, publicKeyURL.absoluteString, signatureBase64, saltBase64, timestampString, copy_args );
            }];

            return result;
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
        pybind::def_function_args( _kernel, "iOSGameCenterConnect", &Detail::iOSGameCenter_connect );
        pybind::def_function( _kernel, "iOSGameCenterIsConnect", &Detail::iOSGameCenter_isConnect );
        pybind::def_function_args( _kernel, "iOSGameCenterRequestIdentityVerificationSignature", &Detail::iOSGameCenter_requestIdentityVerificationSignature );
        pybind::def_function_args( _kernel, "iOSGameCenterReportAchievement", &Detail::iOSGameCenter_reportAchievement );
        pybind::def_function( _kernel, "iOSGameCenterCheckAchievement", &Detail::iOSGameCenter_checkAchievement );
        pybind::def_function_args( _kernel, "iOSGameCenterResetAchievements", &Detail::iOSGameCenter_resetAchievements );
        pybind::def_function_args( _kernel, "iOSGameCenterReportScore", &Detail::iOSGameCenter_reportScore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSGameCenterConnect", nullptr );
        _kernel->remove_from_module( "iOSGameCenterIsConnect", nullptr );
        _kernel->remove_from_module( "iOSGameCenterRequestIdentityVerificationSignature", nullptr );
        _kernel->remove_from_module( "iOSGameCenterReportAchievement", nullptr );
        _kernel->remove_from_module( "iOSGameCenterCheckAchievement", nullptr );
        _kernel->remove_from_module( "iOSGameCenterResetAchievements", nullptr );
        _kernel->remove_from_module( "iOSGameCenterReportScore", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
