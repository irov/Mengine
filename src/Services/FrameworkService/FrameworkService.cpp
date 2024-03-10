#include "FrameworkService.h"

#include "Interface/FrameworkFactoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FrameworkService, Mengine::FrameworkService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FrameworkService::FrameworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FrameworkService::~FrameworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkService::_finalizeService()
    {
#if defined(MENGINE_DEBUG)
        for( const FrameworkInterfacePtr & framework : m_frameworks )
        {
            LOGGER_ASSERTION( "was forgotten stop framework '%s'"
                , framework->getName().c_str()
            );
        }
#endif

        m_frameworks.clear();

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Framework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::hasFramework( const ConstString & _name ) const
    {
        bool exist = VOCABULARY_HAS( STRINGIZE_STRING_LOCAL( "Framework" ), _name );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::initializeFramework( const ConstString & _name, const DocumentInterfacePtr & _doc )
    {
        FrameworkFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Framework" ), _name );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not found framework '%s'"
            , _name.c_str() 
        );

        FrameworkInterfacePtr framework = factory->createFramework( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( framework, "invalid create framework '%s'"
            , _name.c_str() 
        );

        framework->setName( _name );

        if( framework->initializeFramework() == false )
        {
            LOGGER_ERROR( "invalid initialize framework '%s'"
                , _name.c_str()
            );

            return false;
        }

        m_frameworks.emplace_back( framework );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::finalizeFramework( const ConstString & _name )
    {
        FrameworkInterfacePtr framework = this->popFramework_( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( framework, "not found framework '%s'"
            , _name.c_str()
        );

        framework->finalizeFramework();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::runFrameworks()
    {
        for( const FrameworkInterfacePtr & framework : m_frameworks )
        {
            if( framework->runFramework() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::stopFrameworks()
    {
        VectorFrameworks reverse_frameworks = m_frameworks;
        Algorithm::reverse( reverse_frameworks.begin(), reverse_frameworks.end() );

        for( const FrameworkInterfacePtr & framework : reverse_frameworks )
        {
            framework->stopFramework();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkService::isInitializeFramework( const ConstString & _name ) const
    {
        VectorFrameworks::const_iterator it_found =
            Algorithm::find_if( m_frameworks.begin(), m_frameworks.end(), [&_name]( const FrameworkInterfacePtr & _framework )
        {
            return _framework->getName() == _name;
        } );

        if( it_found == m_frameworks.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FrameworkInterfacePtr & FrameworkService::findFramework_( const ConstString & _name ) const
    {
        VectorFrameworks::const_iterator it_found =
            Algorithm::find_if( m_frameworks.begin(), m_frameworks.end(), [&_name]( const FrameworkInterfacePtr & _framework )
        {
            return _framework->getName() == _name;
        } );

        if( it_found == m_frameworks.end() )
        {
            return FrameworkInterfacePtr::none();
        }

        const FrameworkInterfacePtr & framework = *it_found;

        return framework;
    }
    //////////////////////////////////////////////////////////////////////////
    FrameworkInterfacePtr FrameworkService::popFramework_( const ConstString & _name )
    {
        VectorFrameworks::iterator it_found =
            Algorithm::find_if( m_frameworks.begin(), m_frameworks.end(), [&_name]( const FrameworkInterfacePtr & _framework )
        {
            return _framework->getName() == _name;
        } );

        if( it_found == m_frameworks.end() )
        {
            return nullptr;
        }

        FrameworkInterfacePtr framework = *it_found;

        m_frameworks.erase( it_found );

        return framework;
    }
    //////////////////////////////////////////////////////////////////////////
}