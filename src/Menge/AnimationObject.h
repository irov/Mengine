#	pragma once

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat4.h"

#	include "math/quat.h"

#	include <cal3d\global.h>
#	include <cal3d\cal3d.h>
#	include <cal3d\cal3d_wrapper.h>
#	include <cal3d\coretrack.h>

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

		void _update( float _timing ) override;

		bool isVisible( const Camera3D * _camera ) override;

	public:
		void play(const std::string& _name);
		void play2Blend(const std::string& _name1,	float _weight1,
			const std::string& _name2,	float _weight2
		);
		void executeAction(const std::string & _name, float _timeIn,
			float _timeOut, float _width, bool _autoLock
		);
	
		mt::mat4f	getBoneWorldMatrix(int _index);

		void setCallback(const std::string & _name, float _updateTime, UpdateCallback _updateCallback, CompleteCallback _completeCallback, void * _userData);
		void clearCallback(const std::string & _name);

		// cal3d specs:
		bool		isSimilarModel(const CalModel * _calModel);

	protected:
		bool _activate() override;
		void _deactivate() override;
	

		void _render( const mt::mat4f & _rwm, const Camera3D * _camera ) override;
		void _debugRender() override;
	
	private:
		typedef std::map<int, AnimationCallback*> TMapCallbacks;
		typedef std::list<std::pair<int, AnimationCallback*> > TListRemoveCallbacks;

		std::string m_resourceName;

		TMapCallbacks	m_callbacks;
		TListRemoveCallbacks	m_removeCallbacks;		

		size_t m_vertexSize;

		ResourceAnimationCal3d * m_cal3dRes;
		VertexDeclaration * m_vertexDecl;
		PrimitiveData * m_primitiveData;
		CalModel * m_calModel;

		void	_initGeometry();
		void	_clearRemovedCallback();
		void	_clearCycles(float _blendTime);
	};
}
