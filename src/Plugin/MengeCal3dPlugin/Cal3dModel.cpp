#	include "Cal3dModel.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Cal3dModel::Cal3dModel()
		: m_cal3dCoreModel(nullptr)
		, m_calModel(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Cal3dModel::~Cal3dModel()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dModel::setCoreModel( Cal3dCoreModel * _coreModel )
	{
		m_cal3dCoreModel = _coreModel;
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dModel::attachMesh( const ConstString & _name )
	{
		m_attachMeshes.push_back( _name );

		if( this->isCompile() == false )
		{
			return;
		}

		int meshId = m_cal3dCoreModel->getMeshId( _name );

		m_calModel->attachMesh( meshId );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Cal3dModel::_compile()
	{
		if( m_cal3dCoreModel == nullptr )
		{
			return false;
		}

		m_calModel = m_cal3dCoreModel->createModel();

		for( TVectorConstString::const_iterator 
			it = m_attachMeshes.begin(),
			it_end = m_attachMeshes.end();
		it != it_end;
		++it )
		{
			const ConstString & meshName = *it;

			int meshId = m_cal3dCoreModel->getMeshId( meshName );

			m_calModel->attachMesh( meshId );
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dModel::_release()
	{
		m_cal3dCoreModel->destroyModel( m_calModel );
		m_calModel = nullptr;

		m_cal3dCoreModel = nullptr;		
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dModel::_update( float _current, float _timing )
	{
		m_calModel->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dModel::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{

	}
}