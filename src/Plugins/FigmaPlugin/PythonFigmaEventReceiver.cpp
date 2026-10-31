#include "PythonFigmaEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFigmaEventReceiver::PythonFigmaEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFigmaEventReceiver::~PythonFigmaEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaTrigger( const FigmaTriggerEvent & _event )
    {
        m_cb.call( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaAction( const FigmaActionEvent & _event, FigmaActionResponse * const _response )
    {
        pybind::kernel_interface * kernel = m_cb.kernel();

        pybind::object response = pybind::make_object_t( kernel, *_response );

        m_cb.call( _event, response );

        *_response = pybind::extract<FigmaActionResponse>( kernel, response.ptr() );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaFrameChanged( const String & _previousFrameId, const String & _currentFrameId )
    {
        m_cb.call( _previousFrameId, _currentFrameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaOverlayOpened( const String & _frameId )
    {
        m_cb.call( _frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaOverlayClosed( const String & _frameId )
    {
        m_cb.call( _frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFigmaEventReceiver::onFigmaStateChanged( const String & _sourceNodeId, const String & _previousStateId, const String & _currentStateId )
    {
        m_cb.call( _sourceNodeId, _previousStateId, _currentStateId );
    }
    //////////////////////////////////////////////////////////////////////////
}
