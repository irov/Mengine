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

	class ResourceVisitor
	{
		public:
			virtual void visit(ResourceReference* _resource) = 0;
			virtual void visit(ResourceImage* _resource) = 0;
			virtual void visit(ResourceImageDefault* _resource) = 0;
			virtual void visit(ResourceImageSet* _resource) = 0;
			virtual void visit(ResourceImageCell* _resource) = 0;
	};
};