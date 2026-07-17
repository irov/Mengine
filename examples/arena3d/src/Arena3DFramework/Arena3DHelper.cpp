#include "Arena3DHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f kinefixVec3ToVec3f( const kf_vec3_t & _value )
        {
            return mt::vec3f(
                kf_fixed_to_float( _value.x ),
                kf_fixed_to_float( _value.y ),
                kf_fixed_to_float( _value.z ) );
        }
        //////////////////////////////////////////////////////////////////////////
        float kinefixAngleToRadians( kf_angle16_t _angle )
        {
            return kf_fixed_to_float( kf_angle16_to_fixed_radians( _angle ) );
        }
        //////////////////////////////////////////////////////////////////////////
        float kinefixAngleToRadians( kf_sangle16_t _angle )
        {
            return kf_fixed_to_float( kf_sangle16_to_fixed_radians( _angle ) );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
