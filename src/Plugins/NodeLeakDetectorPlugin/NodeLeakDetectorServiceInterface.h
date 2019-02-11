#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class NodeLeakDetectorServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NodeLeakDetectorService" );

    public:
        virtual void increfGeneration() = 0;
        virtual uint32_t getGeneration() const = 0;

        typedef Lambda<void( const Factory * _factory, const Factorable *, const Char * _doc )> LambdaNodeLeaks;
        virtual void getNodeLeak( uint32_t _generation, const LambdaNodeLeaks & _leaks ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#   define NODELEAKDETECTOR_SERVICE()\
	((NodeLeakDetectorServiceInterface *)SERVICE_GET(Mengine::NodeLeakDetectorServiceInterface))
//////////////////////////////////////////////////////////////////////////