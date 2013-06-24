#   include "ProfilerService.h"

#   include "Interface/ApplicationInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ProfilerService, Menge::ProfilerServiceInterface, Menge::ProfilerService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ProfilerService::ProfilerService()
        : m_serviceProvider(nullptr)
        , m_checkpoint(nullptr)
        , m_beginMemoryUsage(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerService::~ProfilerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ProfilerService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ProfilerService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ProfilerService::initialize()
    {
        //m_beginMemoryUsage = PLATFORM_SERVICE(m_serviceProvider)
        //    ->getMemoryUsage();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ProfilerService::memoryBegin()
    {
        m_checkpoint = PLATFORM_SERVICE(m_serviceProvider)
            ->checkpointMemory();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ProfilerService::memoryEnd()
    {
        size_t memory = PLATFORM_SERVICE(m_serviceProvider)
            ->diffMemory( m_checkpoint );

        m_checkpoint = nullptr;

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ProfilerService::getMemoryUsage() const
    {
        size_t memory = PLATFORM_SERVICE(m_serviceProvider)
            ->getMemoryUsage();

        size_t memory_pr = memory - m_beginMemoryUsage;
        float mb = float(memory_pr) / (1024.f * 1024.f);

        return memory_pr;
    }
}