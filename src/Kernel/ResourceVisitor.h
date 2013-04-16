#	pragma once

//#	include "Kernel/ResourceReference.h"
//#	include "ResourceImageDefault.h"

namespace Menge
{
	class ResourceReference;
	class ResourceImageDefault;
	class ResourceImageSet;
	class ResourceImageCell;

	class ResourceAnimation;
	class ResourceEmitterContainer;
	class ResourceFont;
	class ResourcePlaylist;
	class ResourceWindow;
	//class ResourceTilePolygon;
	class ResourceVideo;
	class ResourceSound;
	class ResourceMeshMS3D;
	class ResourceMaterial;
	class ResourceMeshNoise;
	class ResourcePlaylist;
    class ResourceMovie;
				
	class ResourceVisitor
	{
		public:
			virtual void visit(ResourceReference* _resource) { (void)_resource; };
			virtual void visit(ResourceAnimation* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceEmitterContainer* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceFont* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourcePlaylist* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceSound* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceWindow* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceVideo* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
            virtual void visit(ResourceMovie* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceMeshMS3D* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			//virtual void visit(ResourceTilePolygon* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceImageDefault* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceImageSet* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceImageCell* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceMaterial* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
			virtual void visit(ResourceMeshNoise* _resource) { this->visit( (ResourceReference*)( _resource ) ); };
	};
};
