#	pragma once

#	include "Kernel/Node.h"

#   include "ResourceMovie.h"

namespace Menge
{
	class MovieNodeExtra
        : public Node
	{
    public:
        void movieForwardUpdate( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer & _layer );

    protected:
        virtual void _movieForwardUpdate( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer & _layer ) = 0;
	};
}