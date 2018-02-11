#	include "RenderCamera.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderCamera::RenderCamera()
		: m_observerChangeWindowResolution( nullptr )		
		, m_invalidateProjectionMatrix( true )
		, m_invalidateViewMatrix( true )
		, m_invalidateViewProjectionMatrix( true )	
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderCamera::_activate()
	{
        if( Node::_activate() == false )
		{
			return true;
		}

		m_observerChangeWindowResolution = NOTIFICATION_SERVICE()
			->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &RenderCamera::notifyChangeWindowResolution );

		this->invalidateViewMatrix_();
		this->invalidateProjectionMatrix_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::_deactivate()
	{
        Node::_deactivate();

		m_observerChangeWindowResolution = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateViewMatrix_();
		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::updateViewProjectionMatrix_() const
	{
		m_invalidateViewProjectionMatrix = false;

		const mt::mat4f & vm = this->getCameraViewMatrix();
		const mt::mat4f & pm = this->getCameraProjectionMatrix();

		mt::mul_m4_m4( m_viewProjectionMatrix, vm, pm );
        mt::inv_m4_m4( m_viewProjectionMatrixInv, m_viewProjectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateProjectionMatrix_();
	}
}
