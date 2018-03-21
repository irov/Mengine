#	pragma once

#	include "ResourceCal3dSkeleton.h"
#	include "ResourceCal3dMesh.h"
#	include "ResourceCal3dAnimation.h"

#	include "cal3d/coremodel.h"
#	include "cal3d/model.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class Cal3dCoreModel
	{
	public:
		Cal3dCoreModel();
		~Cal3dCoreModel();

	public:
		void setResourceSkeleton( ResourceCal3dSkeleton * _resource );

		void addResourceMesh( const ConstString & _name, ResourceCal3dMesh * _resource );
		void addResourceAnimation( const ConstString & _name, ResourceCal3dAnimation * _resource );

	public:
		int getMeshId( const ConstString & _name ) const;
		int getAnimationId( const ConstString & _name ) const;

	public:
		bool compile();
		void release();

	public:
		CalModel * createModel();
		void destroyModel( CalModel * _model );

	protected:
		ResourceHolder<ResourceCal3dSkeleton> m_resourceSkeleton;

		struct Cal3dMeshDesc
		{
			ResourceHolder<ResourceCal3dMesh> resource;
			int meshId;
		};

		typedef std::map<ConstString, Cal3dMeshDesc> TMapResourceHolderMesh;
		TMapResourceHolderMesh m_resourceMeshes;

		struct Cal3dAnimationDesc
		{
			ResourceHolder<ResourceCal3dAnimation> resource;
			int animationId;
		};
		
		typedef std::map<ConstString, Cal3dAnimationDesc> TMapResourceHolderAnimations;
		TMapResourceHolderAnimations m_resourceAnimations;

		CalCoreModel * m_calCoreModel;
	};
}