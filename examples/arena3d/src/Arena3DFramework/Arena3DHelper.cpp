#include "Arena3DHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        float kinefixFixedToFloat( kf_fixed_t _value )
        {
            return static_cast<float>(kf_fixed_to_double( _value ));
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f kinefixVec3ToVec3f( const kf_vec3_t & _value )
        {
            return mt::vec3f(
                kinefixFixedToFloat( _value.x ),
                kinefixFixedToFloat( _value.y ),
                kinefixFixedToFloat( _value.z ) );
        }
        //////////////////////////////////////////////////////////////////////////
        float kinefixAngleToRadians( kf_angle16_t _angle )
        {
            return kinefixFixedToFloat( kf_angle16_to_fixed_radians( _angle ) );
        }
        //////////////////////////////////////////////////////////////////////////
        float kinefixAngleToRadians( kf_sangle16_t _angle )
        {
            return kinefixFixedToFloat( kf_sangle16_to_fixed_radians( _angle ) );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
