#	pragma once

namespace Utility
{

	template<class T_NODE>
	class CRefCountId
	{
		class CClientNode
		{
		public:
			inline ~CClientNode()
			{
				Release();
			}

			inline CClientNode()
				:ServerNode(0)
			{
			}

			inline CClientNode(const CClientNode &client)
				:ServerNode(client.ServerNode)
			{
				_Copy();
			}

		private:
			inline CClientNode(CRefCountId *server)
				:	ServerNode(server)
			{
				_Copy();
			}

			inline void _Copy()
			{
				if( ServerNode )
				{
					ServerNode->_IncRef();
				}
			}
		public:

			inline void Release()
			{
				if( ServerNode )
				{
					ServerNode->_DecRef();
					ServerNode = 0;
				}
			}

			inline  T_NODE * Ptr()
			{
				return static_cast<T_NODE*>(ServerNode);
			}

		private:
			CRefCountId *ServerNode;

			friend class CRefCountId;
			friend class TypeId;
		};

	private:
		inline void _IncRef()
		{
			++m_RefCount;
		}

		inline void _DecRef()
		{
			--m_RefCount;
		}

	public:

		class TypeId
		{
		public:
			inline TypeId()
			: Id(0)
			{
			}

			inline TypeId( CRefCountId *RefId )
			: Id(new CClientNode(RefId))
			{
			}

			inline  void Release()
			{
				if( Id )
				{
					Id->Release();
					delete Id;
					Id = 0;
				}
			}

			inline bool IsValid()const
			{
				return (Id != 0);
			}

			inline bool operator == (const TypeId &in)const
			{
				return Id == in.Id;
			}

			bool operator != (const TypeId &in)const
			{
				return ! operator == (in);
			}

			inline T_NODE * Ptr()
			{
				return Id->Ptr();
			}

			inline  T_NODE * operator -> ()
			{
				return Ptr();
			}

		private:
			CClientNode *Id;
			friend class CRefCountId;
		};

		inline CRefCountId()
			: m_RefCount(0)
		{
		}

		inline virtual ~CRefCountId()
		{
		}

		inline size_t GetRefCount()const
		{
			return m_RefCount;
		}

		inline TypeId GetId()
		{
			return TypeId(this);
		}

		inline T_NODE * Ptr()
		{
			return static_cast<T_NODE*>(this);
		}

		static TypeId GenInvalidId()
		{
			return TypeId();
		}

	private:
		unsigned int m_RefCount;
	};



}
