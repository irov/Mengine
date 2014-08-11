#	pragma once

#	include "Kernel/Node.h"

#	include "Cal3dCoreModel.h"

#	include "cal3d/coremodel.h"
#	include "cal3d/model.h"

namespace Menge
{
	class Cal3dModel
		: public Node
	{
	public:
		Cal3dModel();
		~Cal3dModel();

	public:
		void setCoreModel( Cal3dCoreModel * _coreModel );

	public:
		void attachMesh( const ConstString & _name );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

	protected:
		Cal3dCoreModel * m_cal3dCoreModel;

		CalModel * m_calModel;

		TVectorConstString m_attachMeshes;
	};
}