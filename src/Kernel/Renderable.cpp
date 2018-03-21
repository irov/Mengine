#include "Kernel/Renderable.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
		, m_localHide(false)
        , m_externalRender(false)
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    Renderable::~Renderable()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_render( RenderServiceInterface * _renderService, const RenderObjectState * _state )
	{
		(void)_renderService;
		(void)_state;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_debugRender( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask )
	{
		(void)_renderService;
		(void)_state;
		(void)_debugMask;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::setHide( bool _hide )
	{
		if( m_hide == _hide )
		{
			return;
		}

		m_hide = _hide;

		this->_setHide( _hide );
	}
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_setHide( bool _hide )
    {
        (void)_hide;
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	void Renderable::setLocalHide( bool _localHide )
	{
		if( m_localHide == _localHide )
		{
			return;
		}

		m_localHide = _localHide;

		this->_setLocalHide( _localHide );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_setLocalHide( bool _localHide )
	{
        (void)_localHide;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void Renderable::setExternalRender( bool _externalRender )
    {
        if( m_externalRender == _externalRender )
        {
            return;
        }

        m_externalRender = _externalRender;

        this->_setExternalRender( _externalRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void Renderable::_setExternalRender( bool _externalRender )
    {
        (void)_externalRender;
        //Empty
    }
}
