#	pragma once

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat4.h"

#	include "math/quat.h"

#	include <cal3d\cal3d.h>

namespace Menge
{
	class AnimationCallback;
	class AnimationObject;
	class ResourceAnimationCal3d;

	typedef void (*UpdateCallback)(AnimationObject * _animObj, const std::string & _animName, float _animTime, void * _userData);
	typedef void (*CompleteCallback)(AnimationObject * _animObj, const std::string & _animName, void * _userData);

	class AnimationObject
		: public SceneNode3D
	{
		OBJECT_DECLARE(AnimationObject);
	public:
		AnimationObject();
		~AnimationObject();

	public:
		void loader( TiXmlElement * _xml ) override;
		void update( float _timing ) override;

		virtual bool isVisible( const Camera3D * _camera ) override;

	public:
		void clearCycles();
		void play(const std::string& _name);
		void play2Blend(const std::string& _name1,	float _weight1,
			const std::string& _name2,	float _weight2
		);
		// 
		int getBoneIndex(const std::string& _bonename);
		//
		void setCallback(const std::string & _name, float _updateTime, UpdateCallback _updateCallback, CompleteCallback _completeCallback, void * _userData);
		void clearCallback(const std::string& _name);
		void clearRemovedCallback();

		CalModel * m_calModel;

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat4f & _rwm, const Camera3D * _camera );
		void _debugRender() override;
	
	private:
		typedef std::map<int, AnimationCallback*> TMapCallbacks;
		typedef std::list<std::pair<int, AnimationCallback*> > TListRemoveCallbacks;

		TMapCallbacks	m_callbacks;
		TListRemoveCallbacks	m_removeCallbacks;		

		std::string m_resourceName;

		ResourceAnimationCal3d * m_cal3dRes;

		size_t m_vertexSize;

		VertexDeclaration * m_vertexDecl;
		PrimitiveData * m_primitiveData;

		void	_initGeometry();
	};
}