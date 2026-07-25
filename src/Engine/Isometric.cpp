#include "Isometric.h"

#include "Interface/TransformationInterface.h"

#include <cmath>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Isometric::Isometric()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Isometric::~Isometric()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class FIsometricSortY
        {
        public:
            bool operator() ( const NodePtr & _left, const NodePtr & _right ) const
            {
                TransformationInterface * leftTransformation = _left->getTransformation();
                TransformationInterface * rigthTransformation = _right->getTransformation();

                const mt::vec3f & l = leftTransformation->getLocalPosition();
                const mt::vec3f & r = rigthTransformation->getLocalPosition();

                float l_y_bucket = std::floor( l.y * 10.f );
                float r_y_bucket = std::floor( r.y * 10.f );

                if( l_y_bucket != r_y_bucket )
                {
                    return l_y_bucket < r_y_bucket;
                }

                if( l.x != r.x )
                {
                    return l.x < r.x;
                }

                return l.y < r.y;
            }
        };
    }
    ////////////////////////////////////////////////////////////////////////
    void Isometric::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        stdex::helper::intrusive_sort_stable( m_children, FIsometricSortY() );
    }
    //////////////////////////////////////////////////////////////////////////
}
