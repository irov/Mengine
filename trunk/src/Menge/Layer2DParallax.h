#	pragma once

#	include "Kernel/Layer.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;

	class ProxyCameraParallax
		: public RenderCameraInterface
	{
	public:
		ProxyCameraParallax()
			: m_camera(nullptr)
			, m_parallax(1.f, 1.f)
		{
		}

	public:
		void setup( const RenderCameraInterface * _camera, const mt::vec2f & _parallax )
		{
			m_camera = _camera;
			m_parallax = _parallax;
		}

	protected:
		const mt::mat4f & getProjectionMatrix() const override
		{
			const mt::mat4f & pm = m_camera->getProjectionMatrix();

			return pm;
		}

		const mt::mat4f & getViewMatrix() const override
		{
			m_vm = m_camera->getViewMatrix();

			m_vm.v3.x *= m_parallax.x;
			m_vm.v3.y *= m_parallax.y;

			return m_vm;
		}

		const Viewport & getRenderport() const override
		{
			const Viewport & renderport = this->getRenderport();

			return renderport;
		}

	protected:
		const ConstString & getRenderTarget() const override
		{
			const ConstString & rt = m_camera->getRenderTarget();

			return rt;
		}

	protected:
		bool isOrthogonalProjection() const override
		{
			bool orthogonal = m_camera->isOrthogonalProjection();

			return orthogonal;
		}

	protected:
		const RenderCameraInterface * m_camera;
		mt::vec2f m_parallax;

		mutable mt::mat4f m_vm;
	};

	class Layer2DParallax
		: public Layer
	{
        DECLARE_VISITABLE()

	public:
		Layer2DParallax();

	public:
		void setParallaxFactor( const mt::vec2f & _parallax );
		const mt::vec2f & getParallaxFactor() const;

	protected:
		void createRenderViewport_();
		void removeRenderViewport_();

	public:
		mt::vec2f cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point );

		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;
    
    protected:
		void _addChildren( Node * _node ) override;

	protected:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:
		mt::vec2f m_parallax;

		ProxyCameraParallax * m_proxyCamera2D;
	};
}
