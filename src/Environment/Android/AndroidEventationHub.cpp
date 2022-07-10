#include "AndroidEventationHub.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    AndroidEventationHub::AndroidEventationHub()
    {
    }
    ////////////////////////////////////////////////////////////////////
    AndroidEventationHub::~AndroidEventationHub()
    {
    }
    ////////////////////////////////////////////////////////////////////
    bool AndroidEventationHub::initialize()
    {
        //Empty

        return true;
    }
    ////////////////////////////////////////////////////////////////////
    void AndroidEventationHub::finalize()
    {
        m_eventations.clear();
    }
    ////////////////////////////////////////////////////////////////////
    void AndroidEventationHub::addAndroidEventation( const AndroidEventationInterfacePtr & _eventation )
    {
        m_eventations.push_back( _eventation );
    }
    ////////////////////////////////////////////////////////////////////
    void AndroidEventationHub::removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation )
    {
        VectorEventations::iterator it_found = Algorithm::find( m_eventations.begin(), m_eventations.end(), _eventation );

        m_eventations.erase( it_found );
    }
    ////////////////////////////////////////////////////////////////////
    void AndroidEventationHub::invoke()
    {
        for( const AndroidEventationInterfacePtr & eventation : m_eventations )
        {
            eventation->invoke();
        }
    }
    /////////////////////////////////////////////////////////////////
}