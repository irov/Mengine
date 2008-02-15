#	pragma once

#	include <NxPhysics.h>

class NovodexGeometry : public GeometryInterface
{
public:
	NxShapeDesc * nxShape;
	NovodexGeometry(NxShapeDesc * _shape) : nxShape(_shape){};
	~NovodexGeometry() 
	{
		delete nxShape;
	};
private:
};