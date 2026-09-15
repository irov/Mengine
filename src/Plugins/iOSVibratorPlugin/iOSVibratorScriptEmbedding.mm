#include "iOSVibratorScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "iOSVibratorPlugin.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSVibrator_prepare()
        {
            [[iOSVibratorPlugin sharedInstance] prepare];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSVibrator_impact( float _intensity, float _sharpness )
        {
            [[iOSVibratorPlugin sharedInstance] impact:_intensity sharpness:_sharpness];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSVibrator_mute( bool _mute )
        {
            [[iOSVibratorPlugin sharedInstance] mute:_mute];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSVibrator_isMute()
        {
            bool mute = [[iOSVibratorPlugin sharedInstance] isMute];

            return mute;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSVibratorScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "iOSVibratorPrepare", &Detail::iOSVibrator_prepare );
        pybind::def_function( _kernel, "iOSVibratorImpact", &Detail::iOSVibrator_impact );
        pybind::def_function( _kernel, "iOSVibratorMute", &Detail::iOSVibrator_mute );
        pybind::def_function( _kernel, "iOSVibratorIsMute", &Detail::iOSVibrator_isMute );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSVibratorScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSVibratorPrepare", nullptr );
        _kernel->remove_from_module( "iOSVibratorImpact", nullptr );
        _kernel->remove_from_module( "iOSVibratorMute", nullptr );
        _kernel->remove_from_module( "iOSVibratorIsMute", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
