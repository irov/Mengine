#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Animation;

	//! AnimationGroup - ������������ ������. 

    /*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "AnimationGroup">
	*		<Animations>
	*			<Node Name = "���_����0" Type = "Animation">
	*				<ImageMap Name = "���_�������_�������"/>
	*				<Animation Name = "���_�������_��������"/>
	*				<AutoStart Value = "1/0"/>
	*				<Looping Value = "1/0"/>
	*				<Transformation Value = "1;0;0;1;512;0"/>
	*			</Node>
	*			 ...
	*			<Node Name = "���_����N" Type = "Animation">
	*				<ImageMap Name = "���_�������_�������"/>
	*				<Animation Name = "���_�������_��������"/>
	*				<AutoStart Value = "1/0"/>
	*				<Looping Value = "1/0"/>
	*				<Transformation Value = "1;0;0;1;512;0"/>
	*			</Node>
	*		</Animations>
	*	</Node>
	*/

	class AnimationGroup
		: public SceneNode2D
	{
		OBJECT_DECLARE(AnimationGroup)
	public:
		AnimationGroup();

	public:
		//! ��������� ��������.
		/*!
			param _name �������� ��������
		*/
		void play( const std::string & _name );

		//! ���������� ��������.
		/*!
		*/
		void stop();

		//! ��������� �� ����� ��������.
		/*!
		*/
		void pause();

		//! ��������� �������������.
		/*!
		\param _loop ���� ������������.
		*/
		void setLooped( bool _loop );

		//! ���������� ���� ������������
		/*!
		\return ���� ������������
		*/
		bool getLooped() const;

		//! ��������� ���������.
		/*!
		\param _listener ���������
		*/
		void setAnimationListener( PyObject * _listener );

		//! ��������� ������� ��������.
		/*!
		\param _resource ��� �������.
		*/
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