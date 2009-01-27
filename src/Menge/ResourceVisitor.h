#	pragma once

//#	include "ResourceReference.h"
//#	include "ResourceImageDefault.h"

namespace Menge
{
	class ResourceReference;
	class ResourceImage;
	class ResourceImageDefault;
	class ResourceImageSet;
	class ResourceImageCell;

	class ResourceAnimation;
	class ResourceEmitterContainer;
	class ResourceFont;
	class ResourcePlaylist;
	class ResourceWindow;
	class ResourceTilePolygon;
			
	class ResourceVisitor
	{
		public:
			virtual void visit(ResourceAnimation* _resource) = 0;
			virtual void visit(ResourceEmitterContainer* _resource) = 0;
			virtual void visit(ResourceFont* _resource) = 0;

			virtual void visit(ResourcePlaylist* _resource) = 0;

			virtual void visit(ResourceWindow* _resource) = 0;

			virtual void visit(ResourceTilePolygon* _resource) = 0;
									
			virtual void visit(ResourceReference* _resource) = 0;
			virtual void visit(ResourceImage* _resource) = 0;
			virtual void visit(ResourceImageDefault* _resource) = 0;
			virtual void visit(ResourceImageSet* _resource) = 0;
			virtual void visit(ResourceImageCell* _resource) = 0;
	};
};
