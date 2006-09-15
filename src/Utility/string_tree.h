#	pragma once

#	include <string>
#	include <map>

namespace Utility
{
	template<typename T>
	class string_tree
	{
	private:
		typedef std::string TKey;

	public:
		typedef T * value_ptr;
		typedef const T * const_value_ptr;

	private:
		class string_tree_map
		{
			typedef std::map<char, string_tree_map > TMapStringTreeRecursion;
			typedef std::map<char,T> TMapStringTreeValue;

		public:
			typedef const string_tree_map * tree_ptr;

			string_tree_map()
				:m_Key(0)
			{
			}

			~string_tree_map()
			{
				delete [] m_Key;
			}

		public:
			template<typename T_Container>
				std::size_t get_all_key( T_Container &Container)const
			{
				std::size_t Count = 0;
				if( m_StringTreeRecursion.empty() == false )
				{
					TMapStringTreeRecursion::const_iterator it_end = 
						m_StringTreeRecursion.end();

					for( TMapStringTreeRecursion::const_iterator it =
						m_StringTreeRecursion.begin();
						it!=it_end;
					++it)
					{
						Count += it->second.get_all_key(Container);
					}
				}

				if( m_StringTreeValue.empty() == false )
				{
					TMapStringTreeValue::const_iterator it_end =
						m_StringTreeValue.end();

					for( TMapStringTreeValue::const_iterator it = 
						m_StringTreeValue.begin();
						it!=it_end;
					++it)
					{
						const char AddKey = it->first;
						Container.push_back(std::string(m_Key) + AddKey);
						++Count;
					}
				}
				return Count;
			}

			template<typename T_Container>
				std::size_t get_all_value( T_Container &Container)const
			{
				std::size_t Count = 0;
				if( m_StringTreeRecursion.empty() == false )
				{
					TMapStringTreeRecursion::const_iterator it_end = 
						m_StringTreeRecursion.end();

					for( TMapStringTreeRecursion::const_iterator it =
						m_StringTreeRecursion.begin();
						it!=it_end;
					++it)
					{
						Count += it->second.get_all_key(Container);
					}
				}

				if( m_StringTreeValue.empty() == false )
				{
					TMapStringTreeValue::const_iterator it_end =
						m_StringTreeValue.end();

					for( TMapStringTreeValue::const_iterator it = 
						m_StringTreeValue.begin();
						it!=it_end;
					++it)
					{
						Container.push_back(&it->second);
						++Count;
					}
				}
				return Count;
			}


			tree_ptr find_sub_tree(const TKey &Key)const
			{
				return find_sub_tree(Key.c_str(),Key.size());
			}

			std::size_t size()const
			{
				std::size_t Size = m_StringTreeValue.size();

				TMapStringTreeRecursion::const_iterator it_end = 
					m_StringTreeRecursion.end();

				for( TMapStringTreeRecursion::const_iterator it =
					m_StringTreeRecursion.begin();
					it!=it_end;
				++it)
				{
					Size += it->second.size();
				}
				return Size;
			}

		private:
			string_tree_map(const char *Key, std::size_t Begin, std::size_t End, const T& Value)
				: m_Key(0)
			{
				insert(Key,Begin,End,Value);
			}

			void insert(const char *Key, std::size_t Begin, std::size_t End, const T& Value)
			{
				if( End == 0 )
				{
					return;
				}

				if( Begin != End )
				{				
					const char _Key = Key[Begin];

					m_StringTreeRecursion[_Key].insert(Key,Begin+1,End,Value);
				}
				else
				{
					if( m_Key == 0 )
					{
						m_Key = new char [Begin+1];
						memcpy(m_Key,Key,Begin);
						m_Key[Begin] = '\0';
					}
					const char _Key = Key[End];
					m_StringTreeValue.insert(std::make_pair(_Key,Value));
				}
			}

			bool erase(const char *Key, std::size_t Size)
			{
				const char _Key = Key[0];	

				if( Size > 1 )
				{
					typename TMapStringTreeRecursion::iterator it_find =
						m_StringTreeRecursion.find(_Key);

					if( it_find == m_StringTreeRecursion.end() )
					{
						return false;
					}

					bool Done = it_find->second.erase(Key+1,Size-1);

					if( Done )
					{
						m_StringTreeRecursion.erase( _Key );

						return m_StringTreeRecursion.empty();
					}

					return false;
				}

				m_StringTreeValue.erase(_Key);

				return m_StringTreeValue.empty();
			}

			const_value_ptr find( const char *Key, std::size_t Size)const
			{
				const char _Key = Key[0];

				if( Size > 1 )
				{				
					typename TMapStringTreeRecursion::const_iterator it_find =
						m_StringTreeRecursion.find(_Key);

					if( it_find == m_StringTreeRecursion.end() )
					{
						return 0;
					}

					return it_find->second.find(Key+1,Size-1);
				}

				TMapStringTreeValue::const_iterator it_find = m_StringTreeValue.find(_Key);

				if( it_find == m_StringTreeValue.end() )
				{
					return 0;
				}

				return &it_find->second;
			}

			value_ptr find( const char *Key, std::size_t Size)
			{
				const char _Key = Key[0];

				if( Size > 1 )
				{				
					typename TMapStringTreeRecursion::iterator it_find =
						m_StringTreeRecursion.find(_Key);

					if( it_find == m_StringTreeRecursion.end() )
					{
						return 0;
					}

					return it_find->second.find(Key+1,Size-1);
				}

				TMapStringTreeValue::iterator it_find = m_StringTreeValue.find(_Key);

				if( it_find == m_StringTreeValue.end() )
				{
					return 0;
				}

				return &it_find->second;
			}

			tree_ptr find_sub_tree(const char *Key, std::size_t Size)const
			{
				if( Size )
				{
					const char _Key = Key[0];
					typename TMapStringTreeRecursion::const_iterator it_find =
						m_StringTreeRecursion.find(_Key);

					if( it_find == m_StringTreeRecursion.end() )
					{
						return 0;
					}

					return it_find->second._find_sub_tree(Key+1,Size-1);
				}
				return 0;
			}

			tree_ptr _find_sub_tree(const char *Key, std::size_t Size)const
			{
				if( Size )
				{
					const char _Key = Key[0];
					typename TMapStringTreeRecursion::const_iterator it_find =
						m_StringTreeRecursion.find(_Key);

					if( it_find == m_StringTreeRecursion.end() )
					{
						return 0;
					}

					return it_find->second._find_sub_tree(Key+1,Size-1);
				}

				return this;
			}

		private:
			TMapStringTreeRecursion m_StringTreeRecursion;
			TMapStringTreeValue m_StringTreeValue;

			char *m_Key;

			friend class string_tree;
		};

	public:
		typedef typename string_tree_map::tree_ptr tree_ptr;

	public:
		std::size_t size()const
		{
			return m_Root.size();
		}

		void insert(const TKey &Key, const T& Value)
		{
			m_Root.insert(Key.c_str(),0,Key.size()-1,Value);
		}

		void erase(const TKey &Key)
		{
			m_Root.erase(Key.c_str(),Key.size());
		}

		const_value_ptr find(const TKey &Key)const
		{
			return m_Root.find(Key.c_str(),Key.size());
		}

		value_ptr find(const TKey &Key)
		{
			return m_Root.find(Key.c_str(),Key.size());
		}

		tree_ptr find_sub_tree(const TKey &Key)const
		{
			return m_Root.find_sub_tree(Key.c_str(),Key.size());
		}

	private:
		string_tree_map m_Root;
	};
}