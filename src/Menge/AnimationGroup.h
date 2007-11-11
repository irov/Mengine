#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Animation;

	class AnimationGroup
		: public SceneNode2D
	{
		OBJECT_DECLARE(AnimationGroup)
	public:
		AnimationGroup();

	public:
		void play( const std::string & _name );
		void pause();
		void stop();

		void setLooped( bool _looped );
		bool getLooped();

		void setAnimationListener( PyObject * _listener );
		void setAnimationResource( const std::string & _resource );

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	private:

		Animation * m_currentAnimation;

		void setCurrentAnimation( const std::string & _name );
	};
}