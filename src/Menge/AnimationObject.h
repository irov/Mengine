#	pragma once

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat4.h"

#	include "math/quat.h"

#	include <cal3d\cal3d.h>

namespace Menge
{
	class AnimationCallback;
	class ResourceAnimationCal3d;

	class AnimationObject;
	
	typedef void (*TUpdateCallback)(AnimationObject *, const std::string & animationName, float time, void * pUserData);
	typedef void (*TCompleteCallback)(AnimationObject *, const std::string & animationName, void * pUserData);

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
		void play2Blend(const std::string& _name1,	float _weight1,
			const std::string& _name2,	float _weight2
		);
		// 
		int getBoneIndex(const std::string& _bonename);
		//
		void getBonePosition(int _boneIndex, mt::vec3f & _position);
		void getBoneRotation(int _boneIndex, mt::quatf & _q);
		
		void setCallback(const std::string & _name, float _updateTime, TUpdateCallback _updateCallback, TCompleteCallback _completeCallback, void * _userData);
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

		VertexDeclaration * m_vertexDecl;
		PrimitiveData * m_primitiveData;

		void	_initGeometry();
	};
}