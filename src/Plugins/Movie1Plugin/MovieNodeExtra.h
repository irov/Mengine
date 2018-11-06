#pragma once

#include "Kernel/Node.h"

#include "ResourceMovie.h"

namespace Mengine
{
    class MovieNodeExtra
        : public Node
    {
        DECLARE_VISITABLE( Node );

    public:
        void movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer );

    protected:
        virtual void _movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieNodeExtra> MovieNodeExtraPtr;
    //////////////////////////////////////////////////////////////////////////
}