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

		bool result = Resource::compile();

		if( result == false )
		{
			printf("Error: compiled Node '%s' is failed\n"
				, getName().c_str() 
				);
		}

		return result;
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

		deactivate();

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeResource::recompile()
	{
		release();
		return compile();
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeResource::isCompile()
	{
		return Resource::isCompile();
	}
}