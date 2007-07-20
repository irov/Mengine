#	pragma once

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat4.h"

#	include "math/quat.h"

#	include <vector>

#	include <cal3d\cal3d.h>

namespace Menge
{
	class ResourceAnimationCal3d;

	class AnimationObject
		: public SceneNode3D
	{
		OBJECT_DECLARE(AnimationObject);
	public:
		AnimationObject();
		~AnimationObject();

	public:
		void loader( TiXmlElement *xml ) override;
		void update( float _timing ) override;

		virtual bool isVisible( const Camera3D * _camera ) override;

	public:
		void clearCycles();
		void play(const std::string& _name);
		void playBlend(
			const std::vector<std::string>& _animNames,
			const std::vector<float>& _weights
		);
		void nextPlay();

		void getBonePosition(int _boneIndex, mt::vec3f& _position);
		int getBoneIndex(const std::string& _bonename);
		void getBoneRotation(int _boneIndex, mt::quatf & _q);
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat4f & _rwm, const Camera3D * _camera );
		void _debugRender() override;
	
	protected:
		CalModel * m_calModel;
	private:
		float	m_scale;
		float	m_leftAnimationTime;
		float	m_blendTime;
		int		m_currentAnimationId;
		
		std::string m_resourceName;

		CalCoreModel * m_calCoreModel;
		ResourceAnimationCal3d * m_cal3dRes;

		VertexDeclaration * m_vertexDecl;
		PrimitiveData * m_primitiveData;

		void	_initGeometry();
	};
}