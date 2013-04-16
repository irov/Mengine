#	include "CURLHTTPSystem.h"
#	include "CURLHTTPRequestPerformer.h"
#	include "CURLHTTPRequestPerformerSync.h"
#	include "CURLHTTPRequestPerformerAsync.h"
#	include "CURLHTTPRequestPerformerPostForm.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::CURLHTTPSystem();

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	CURLHTTPSystem::CURLHTTPSystem() 
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}
	//////////////////////////////////////////////////////////////////////////
	CURLHTTPSystem::~CURLHTTPSystem()
	{
		 curl_global_cleanup();
	}
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * CURLHTTPSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool CURLHTTPSystem::initialize( ServiceProviderInterface * _serviceProvider )
	{	
        (void)_serviceProvider;

		if( m_serviceProvider->registryService( "HTTP", this ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPSystem::destroy()
	{
		m_serviceProvider->unregistryService("HTTP");

		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	HTTPSystemInterface* CURLHTTPSystem::getInterface()
	{
		return this;
	}
    //////////////////////////////////////////////////////////////////////////
	//performers factory
    template <class T>
    static T* createPerformer( const HTTPRequest& _request, HTTPResponseReceiver * _receiver, char * _errorBuffer )
    {
        T* performer = new T(_request);

        performer->addReceiver( _receiver );
        performer->setErrorBuffer( _errorBuffer );

        return performer;
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPSystem::send( const HTTPRequest& _request, HTTPResponseReceiver * _receiver )
	{     
		memset( m_errorBuffer, 0, CURL_HTTP_SYSTEM_ERROR_BUFFER_SIZE );
		
		if( _request.requestType ==  HTTPM_POST )
		{
            CURLHTTPRequestPerformerPostForm * performer =  createPerformer<CURLHTTPRequestPerformerPostForm>( _request, _receiver, m_errorBuffer );
			performer->execute();
			delete performer;
		}
		else
		{
			CURLHTTPRequestPerformerSync * performer =  createPerformer<CURLHTTPRequestPerformerSync>( _request, _receiver, m_errorBuffer );
			performer->execute();
			delete performer;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPSystem::sendAsync( const HTTPRequest& _request, HTTPResponseReceiver * _receiver )
	{
		CURLHTTPRequestPerformerAsync * performer =  createPerformer<CURLHTTPRequestPerformerAsync>( _request, _receiver, m_errorBuffer );
		m_performers.push_back( performer );
		
		while( true, true )
		{
			this->update();

			if( m_performers.empty() == true )
			{
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPSystem::update()
	{
		THTTPRequestPerformersVector::iterator it = m_performers.begin();
		THTTPRequestPerformersVector::iterator it_end = m_performers.end();

		while( it != it_end )
		{
			CURLHTTPRequestPerformer * performer = (*it);
			performer->execute();
			
			if( performer->isComplete() == true )
			{
				delete performer;
				it = m_performers.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
}