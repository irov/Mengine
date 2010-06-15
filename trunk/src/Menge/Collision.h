/*
 *	Collision.h
 *
 *	Created by _Berserk_ on 17.3.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#	include "Node.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "Vertex.h"

namespace Menge
{
	struct Material;

	class Collision
		: public Node
		, public PhysicBody2DListener
	{
	public:
		Collision();
		~Collision();

	public:
		struct ShapePoly
		{
			typedef std::vector<mt::vec2f> TVectorPoints;
			TVectorPoints points;
		};

		typedef std::vector<ShapePoly> TVectorShapePoly;

		struct ShapeBox
		{
			float width;
			float height;
			mt::vec2f position;
			float angle;
		};

		typedef std::vector<ShapeBox> TVectorShapeBox;

		struct ShapeCircle
		{
			float radius;
			mt::vec2f position;
		};

		typedef std::vector<ShapeCircle> TVectorShapeCircle;

		struct CollisionInfo
		{
			Collision* collision;
			bool active;
		};

		typedef std::vector<CollisionInfo> TVectorCollisionInfo;

	protected:
		void onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;
		void onUpdate() override;

	protected:
		void loader( XmlElement* _xml ) override;
		void loaderShapePoly_( XmlElement* _xml, ShapePoly& _shapePoly );
		void loaderShapeCircle_( XmlElement* _xml, ShapeCircle& _shapeCircle );
		void loaderShapeBox_( XmlElement* _xml, ShapeBox& _shapeBox );

	protected:
		void parser( BinParser * _parser ) override;
		void parserShape_( BinParser * _parser );
		void parserShapeCircle_( BinParser * _parser );
		void parserShapeBox_( BinParser * _parser );
		
	protected:
		bool _compile() override;
		void _release() override;
		void _debugRender( Camera2D* _camera, unsigned int _debugMask ) override;
		void _invalidateWorldMatrix() override;
		void _update( float _timing ) override;
		bool _activate() override;
		void _deactivate() override;
		void _setListener( PyObject * _listener ) override;

		void updateTransformation_();
		void updateShapeData_();
		void addShapePolyData_( const ShapePoly& _shapePoly );
		void addShapeBoxData_( const ShapeBox& _shapeBox );
		void addShapeCircleData_( const ShapeCircle& _shapeCircle );

	private:
		PhysicBody2DInterface* m_physicBodyInterface;
		TVectorShapePoly m_shapesPoly;
		TVectorShapeBox m_shapesBox;
		TVectorShapeCircle m_shapesCircle;
		unsigned int m_collisionMask;
		unsigned int m_categoryBits;
		unsigned int m_groupIndex;
		bool m_invalidateTransformation;

		Material* m_shapeMaterial;

		typedef std::vector<TVertex2DVector> TVectorShapeData;
		TVectorShapeData m_shapeData;
		bool m_invalidateShapeData;
		TVectorShapeData m_shapeDataRender;

		TVectorCollisionInfo m_collisions;
	};

}	// namespace Menge
