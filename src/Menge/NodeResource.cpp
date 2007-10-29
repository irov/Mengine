#	include "NodeResource.h"

#	include "NodeForeach.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool NodeResource::compile()
	{
		struct ForeachCompile
			: public NodeForeach
		{
			ForeachCompile()
				: result(true)
			{
			}

			void apply( Node * children ) override
			{
				result = children->compile();
			}

			bool isCompile() const
			{
				return result;
			}

			bool result;
		};

		ForeachCompile functor;
		foreachChildren( functor );
		
		if( functor.isCompile() == false )
		{
			return false;
		}

		return Resource::compile();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeResource::release()
	{
		struct ForeachRelease
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->release();
			}
		};

		foreachChildren( ForeachRelease() );

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeResource::isCompile()
	{
		return Resource::isCompile();
	}
}