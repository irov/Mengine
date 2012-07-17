#	include "RenderParticles.h"
#	include "RenderParticlesScriptWrapper.h"
#	include "EmitterDescriptionManager.h"
#	include "RenderParticlesSuites.h"
#	include "WindowsLayer/WindowsLayer.h"

//////////////////////////////////////////////////////////////////////////
bool initRenderParticles( Menge::RenderParticlesInterface **_system )
{
	try
	{
		*_system = new Menge::RenderParticlesSuites();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseRenderParticles( Menge::RenderParticlesInterface *_system )
{
	delete static_cast<Menge::RenderParticlesSuites*>( _system );
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	RenderParticlesSuites::RenderParticlesSuites()
		: m_render(NULL)
		, m_scriptWrapper(NULL)
		, m_emitterDescriptionManager(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////
	bool RenderParticlesSuites::initialise( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight )
	{
		m_render = new RenderParticles( _hinstance, _windowWidth, _windowHeight );

		if( m_render->initialise() == false )
		{
			return false;
		}

		m_emitterDescriptionManager = new EmitterDescriptionManager();
		EmitterDescriptionManager::keep(m_emitterDescriptionManager);

		m_scriptWrapper = new RenderParticlesScriptWrapper();
		RenderParticlesScriptWrapper::keep(m_scriptWrapper);
		
		WString modulePath;
		WString line;
		WString curDir;

		WindowsLayer::getModuleFileName(NULL,line);
		//WindowsLayer::getCommandLine(line);
		size_t pos = line.find_last_of(L"\\");
		curDir = line.substr(0,pos);

		modulePath = curDir + WString(L"\\Plug-ins\\");
		//modulePath = "c:\\Adobe\\Adobe After Effects CS4\\Support Files\\Plug-ins\\";
		
		ScriptServiceInterface * scriptService = m_render->getScriptService();
		if( m_scriptWrapper->initialise( scriptService, modulePath ) == false )
		{
			return false;
		}
		
		m_scriptWrapper->callFunction( "initMengeEnvironment", "(ii)", _windowWidth, _windowHeight );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::destroy()
	{
		if( m_scriptWrapper != NULL )
		{
			m_scriptWrapper->callFunction("destroyMengeEnvironment","()");
			RenderParticlesScriptWrapper::clear();
			delete m_scriptWrapper;
			m_scriptWrapper = NULL;
		}
		if( m_emitterDescriptionManager != NULL )
		{
			m_emitterDescriptionManager->destroy();
			EmitterDescriptionManager::clear();
			delete m_emitterDescriptionManager;
			m_emitterDescriptionManager = NULL; 
		}
		if( m_render != NULL )
		{
			m_render->finalize();
			delete m_render;
			m_render = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::addListener( RenderParticlesListener * _listener )
	{
		m_scriptWrapper->addListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::removeListener( RenderParticlesListener * _listener )
	{
		m_scriptWrapper->removeListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::loadAstralaxFile( const String& _path )
	{
		WString path;
		WindowsLayer::utf8ToUnicode(_path,path);
		m_scriptWrapper->callFunction( "loadAstralaxFile", "(u)", path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::loadEmitterFromFile( const String& _path, const String & _emitterName, bool _isCached )
	{
		WString path;
		WString emitterName;
		
		WindowsLayer::utf8ToUnicode(_path,path);
		WindowsLayer::utf8ToUnicode(_emitterName,emitterName);
		
		m_scriptWrapper->callFunction( "loadEmitterFromFile", "(uub)"
			, path.c_str()
			, emitterName.c_str()
			, _isCached 
			);
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::loop()
	{
		//For debug
		int i = 0;
		while( true )
		{
			i++;
			m_render->beginRender();
			m_scriptWrapper->callFunction("updateScene","(f)",10.0f);
			m_render->endRender();
			m_render->translateMessage();
		}
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::loadFrame( RenderParticlesFrame * _frame, const TVectorEmitterDescriptionInterface& _emittersToUpdate )
	{
		m_render->beginRender();
		//m_scriptWrapper->callFunction("updateScene","(f)",11);
		for(TVectorEmitterDescriptionInterface::const_iterator
			it = _emittersToUpdate.begin(),
			it_end = _emittersToUpdate.end();
			it != it_end;
			it++)
			{
				int emitterId = (*it)->getEmitterId();
				m_scriptWrapper->callFunction("playEmitter","(if)", emitterId, _frame->time);
			}	
		
		m_scriptWrapper->callFunction("updateScene","(f)",_frame->time);
		
		for(TVectorEmitterDescriptionInterface::const_iterator
			it = _emittersToUpdate.begin(),
			it_end = _emittersToUpdate.end();
		it != it_end;
		it++)
		{
			int emitterId = (*it)->getEmitterId();
			m_scriptWrapper->callFunction("stopEmitter","(i)", emitterId);
		}	
				
		m_render->endRender();
		m_render->getRenderData(_frame);
		
	}
	//////////////////////////////////////////////////////////////////////
	void RenderParticlesSuites::log( const String & _message )
	{
		m_render->log( _message );
	}
	//////////////////////////////////////////////////////////////////////
}