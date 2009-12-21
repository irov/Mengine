#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"

#	include "QuadVertices.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;
	struct Material;
	class Texture;

	//! Sprite - ������. � �������� ������� ��������� ResourceImage, ������� ����� ��������� (� ����������� �� ���� �������, �������� ResourceImageDefault ��� ResourceImageSet) N �����������. ��� ���������� ��� ����, ��� �� ��� ���� �������� �������� ����� ���� ��������� ��������� ����������� � ��������. ������� Animation ����������� �� Sprite � ����� Sprite::getImageIndex �������� ������� ������ �����������.

	/*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Sprite">
	*		<ImageMap Name = "���_�������" />
	*		<ImageIndex Value = "������_��������" />
	*		<CenterAlign Value = "1/0" />
	*		<Scale Value = "value" />
	*	</Node>
	*/

	class Sprite
		: public Node
		, public QuadVertices
	{
		FACTORABLE_DECLARE(Sprite);
	public:
		//! �����������.
		/*!
		*/
		Sprite();

		//! ����������.
		/*!
		*/
		virtual ~Sprite();

	public:
		//! ��������� ������� � ������ �����������.
		/*!
		\param _index ������.
		*/
		virtual void setImageIndex( std::size_t _index );

		//! ���������� ������� ������ � ������ �����������
		/*!
		\return ������
		*/
		std::size_t getImageIndex() const;

		//! ��������� �������.
		/*!
		\param _name ��� �������.
		*/
		void setImageResource( const String& _name );
		void setImageAlpha( const String& _name );

		std::size_t getImageCount();

		mt::vec2f getImageSize();
		bool getCenterAlign() const;

		void setCenterAlign( bool _centerAlign );
		//! ���������� ��� �������
		/*!
		\return ��� �������
		*/
		const String& getImageResource() const;

		//! �������������� ������� �� ��� x, ���� _x = true, ����� �� ��� y.
		/*!
		\param _x ���.
		*/
		void flip( bool _x );

		//! ��������� �������� ��������� ������� �� �����. 0.0 - ��� �����. 1.0 - �� �����.
		/*!
		\param _percentX - ����������� ��������� �� x, y �����.
		\param _percentY - ����������� ��������� �� ������, ������ ������.
		*/
		void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

		void setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb );
		void setPercentVisibilityToStop();

		const mt::vec4f& getPercentVisibility() const;
		void setPercentVisibilityVec4f( const mt::vec4f& _percent );

		void disableTextureColor( bool _disable );

		void setBlendSource( EBlendFactor _src );
		void setBlendDest( EBlendFactor _dst );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _render( Camera2D * _camera ) override;

		void _invalidateWorldMatrix() override;
		void _setListener() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	protected:
		void _updateVertices( Vertex2D * _vertcies, unsigned char _invalidateVertices ) override;

	protected:
		bool compileResource_();

	protected:
		ResourceImage * m_resource;
		String m_resourceName;	
		String m_alphaImageName;
		ResourceImage* m_alphaImage;

		std::size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		mt::vec4f m_percent;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDst;

		Material* m_material;
		bool m_disableTextureColor;
		Texture* m_textures[2];
		int m_texturesNum;
	};
}
