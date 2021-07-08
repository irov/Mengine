#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    struct PathfinderCoordDesc
    {
		uint16_t x;
		uint16_t y;
    };
	//////////////////////////////////////////////////////////////////////////
	struct PathfinderSegmentPathDesc
	{
		uint16_t layer;

		typedef Vector<PathfinderCoordDesc> VectorCoords;
		VectorCoords coords;
	};
	//////////////////////////////////////////////////////////////////////////
	struct PathfinderPathDesc
	{
		typedef Vector<PathfinderSegmentPathDesc> VectorSegmentPaths;
		VectorSegmentPaths segmentPaths;
	};
	//////////////////////////////////////////////////////////////////////////
	class PathfinderMapInterface
		: public ServantInterface
	{
	public:
		virtual uint16_t getWidth() const = 0;
		virtual uint16_t getHeight() const = 0;

	public:
        virtual void setCellLayer( uint16_t _width, uint16_t _height, uint16_t _layer ) = 0;
		virtual uint16_t getCellLayer( uint16_t _width, uint16_t _height ) const = 0;
        virtual void setCellBlock( uint16_t _width, uint16_t _height, bool _block ) = 0;
		virtual bool getCellBlock( uint16_t _width, uint16_t _height ) const = 0;
        virtual void setCellWeight( uint16_t _width, uint16_t _height, float _weight ) = 0;
		virtual float getCellWeight( uint16_t _width, uint16_t _height ) const = 0;

	public:
		virtual bool getPath( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<PathfinderMapInterface> PathfinderMapInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class PathfinderServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE( "Pathfinder" )

	public:
        virtual PathfinderMapInterfacePtr createPathfinderMap( uint16_t _width, uint16_t _height ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define PATHFINDER_SERVICE()\
    ((Mengine::PathfinderServiceInterface *)SERVICE_GET(Mengine::PathfinderServiceInterface))
//////////////////////////////////////////////////////////////////////////
