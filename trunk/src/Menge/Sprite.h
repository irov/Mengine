#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"

#	include "Vertex.h"

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
		virtual std::size_t getImageIndex() const;

		//! ��������� �������.
		/*!
		\param _name ��� �������.
		*/
		virtual void setImageResource( const String& _name );
		void setImageAlpha( const String& _name );

		std::size_t getImageCount() const;

		virtual mt::vec2f getImageSize();
		virtual bool getCenterAlign();

		void setCenterAlign( bool _centerAlign );
		//! ���������� ��� �������
		/*!
		\return ��� �������
		*/
		virtual const String& getImageResource() const;

		//! �������������� ������� �� ��� x, ���� _x = true, ����� �� ��� y.
		/*!
		\param _x ���.
		*/
		virtual void flip( bool _x );

		//! ��������� �������� ��������� ������� �� �����. 0.0 - ��� �����. 1.0 - �� �����.
		/*!
		\param _percentX - ����������� ��������� �� x, y �����.
		\param _percentY - ����������� ��������� �� ������, ������ ������.
		*/
		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

		void setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb );
		void setPercentVisibilityToStop();

		const mt::vec4f& getPercentVisibility() const;
		void setPercentVisibilityVec4f( const mt::vec4f& _percent );

		void disableTextureColor( bool _disable );

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
		void updateVertices();
		void invalidateVertices();

	private:
		void updateSprite_();
		void updateDimensions_( ResourceImage * _resource, bool _wrap );

	protected:
		ResourceImage * m_resource;
		String m_resourceName;	
		String m_alphaImageName;
		ResourceImage* m_alphaImage;

		std::size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;

		bool m_invalidateVertices;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;

		Material* m_material;
		Vertex2D m_vertices[4];
		bool m_disableTextureColor;
		Texture* m_textures[2];
		int m_texturesNum;
	};
}
