#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Factory.h"
#include "Kernel/Factorable.h"

#include "Config/Lambda.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const Factory * _factory, const Factorable *, const Char * _doc )> LambdaNodeLeaks;
    //////////////////////////////////////////////////////////////////////////
    class NodeLeakDetectorServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NodeLeakDetectorService" );

    public:
        virtual void increfGeneration() = 0;
        virtual uint32_t getGeneration() const = 0;
        
        virtual void getNodeLeak( uint32_t _generation, const LambdaNodeLeaks & _leaks ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#   define NODELEAKDETECTOR_SERVICE()\
	((NodeLeakDetectorServiceInterface *)SERVICE_GET(Mengine::NodeLeakDetectorServiceInterface))
//////////////////////////////////////////////////////////////////////////