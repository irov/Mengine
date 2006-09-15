#	pragma once

#	include <map>
#	include <list>

namespace Utility
{
	template<class T_KEY_1,class T_KEY_2,class T_VALUE>
	class CDoubleMap
	{
	public:
		typedef std::list<T_VALUE> TListValue;

	private:
		template<class T_KEY>
		struct key_iterator
		{
			key_iterator()
			{
			}

			key_iterator( const T_KEY &key,const typename TListValue::iterator &it)
				:Key(key)
				,It(it)
			{
			}

			T_KEY Key;
			typename TListValue::iterator It;
		};

	private:
		typedef key_iterator<T_KEY_1> TPairFirstKeyValueIterator;
		typedef key_iterator<T_KEY_2> TPairSecondKeyValueIterator;

		typedef std::map<T_KEY_1,TPairSecondKeyValueIterator> TMapFirstKeyValue;
		typedef std::map<T_KEY_2,TPairFirstKeyValueIterator> TMapSecondKeyValue;
	private:

		template<class T_KEY>
			key_iterator<T_KEY> MakePairKeyValueIterator( 
			const T_KEY &key, 
			const typename TListValue::iterator &it )
		{
			return key_iterator<T_KEY>(key,it);
		}

	public:
		void insert(const T_KEY_1 &key1, const T_KEY_2 &key2, const T_VALUE &value)
		{
			TListValue::iterator it = _Lv.insert(_Lv.end(),value);
			_Fkv.insert(std::make_pair(key1,MakePairKeyValueIterator(key2,it)));
			_Skv.insert(std::make_pair(key2,MakePairKeyValueIterator(key1,it)));
		}

		const T_KEY_2 & otherkey( const T_KEY_1 &key1 )const
		{
			return _Fkv.find(key1)->second.Key;
		}

		const T_KEY_1 & otherkey( const T_KEY_2 &key2 )const
		{
			return _Skv.find(key2)->second.Key;
		}

		void erase(const T_KEY_1 &key1)
		{
			TMapFirstKeyValue::const_iterator it_f = _Fkv.find(key1);

			if( if_end(it_f) == 0 )
			{
				const T_KEY_2 &key2 = it_f->first;
				_Skv.erase(key2);
				_Fkv.erase(it_f);
				_Lv.erase(it_f->It);
			}
		}

		void erase(const T_KEY_2 &key2)
		{
			TMapSecondKeyValue::const_iterator it_f = _Skv.find(key1);

			if( if_end(it_f) == 0 )
			{
				const T_KEY_1 &key1 = it_f->Key;
				_Fkv.erase(key2);
				_Skv.erase(it_f);
				_Lv.erase(it_f->It);
			}		
		}

		typename TListValue::iterator begin()
		{
			return _Lv.begin();
		}

		typename TListValue::iterator end()
		{
			return _Lv.end();
		}

		typename TListValue::const_iterator begin()const
		{
			return _Lv.begin();
		}

		typename TListValue::const_iterator end()const
		{
			return _Lv.end();
		}

		typename TListValue::iterator find(const T_KEY_1 &key1)
		{
			TMapFirstKeyValue::iterator it_f = _Fkv.find(key1);
			if( it_f == _Fkv.end() )
			{
				return _Lv.end();
			}
			return it_f->second.It;
		}

		typename TListValue::iterator find(const T_KEY_2 &key2)
		{
			TMapSecondKeyValue::iterator it_f = _Skv.find(key2);
			if( it_f == _Skv.end() )
			{
				return _Lv.end();
			}
			return it_f->second.It;
		}

		typename TListValue::const_iterator find(const T_KEY_1 &key1)const
		{
			TMapFirstKeyValue::const_iterator it_f = _Fkv.find(key1);
			if( it_f == _Fkv.end() )
			{
				return _Lv.end();
			}
			return it_f->second.It;
		}

		typename TListValue::const_iterator find(const T_KEY_2 &key2)const
		{
			TMapSecondKeyValue::const_iterator it_f = _Skv.find(key2);
			if( it_f == _Skv.end() )
			{
				return _Lv.end();
			}
			return it_f->second.It;
		}

		void change2ndkey( const T_KEY_1 &key1, const T_KEY_2 &key2 )
		{
			TMapFirstKeyValue::iterator it_f = _Fkv.find(key1);
			if( it_f != _Fkv.end() )
			{
				T_KEY_2 &_key2 = it_f->second.Key;
				_Skv[key2] = MakePairKeyValueIterator(key1,it_f->second.It);
				_key2 = key2;
			}
		}

		class data_iterator
		{
		public:	
			data_iterator( typename TMapFirstKeyValue::iterator It )
				: m_Iterator(It)
			{
			}

		public:
			const T_KEY_1 & key_first()
			{
				return m_Iterator->first;
			}

			const T_KEY_2 & key_second()
			{
				return m_Iterator->second.Key;
			}

			T_VALUE &value()
			{
				return *m_Iterator->second.It;
			}

		public:

			data_iterator & operator ++ ()
			{
				++m_Iterator;
				return *this;
			}

			data_iterator & operator ++ (int)
			{
				++m_Iterator;
				return *this;
			}

			bool operator == ( data_iterator It)
			{
				return m_Iterator == It.m_Iterator;
			}

			bool operator != ( data_iterator It)
			{
				return m_Iterator != It.m_Iterator;
			}

		private:
			typename TMapFirstKeyValue::iterator m_Iterator;
		};

		data_iterator dbegin()
		{
			return data_iterator(_Fkv.begin());
		}

		data_iterator dend()
		{
			return data_iterator(_Fkv.end());
		}

	private:
		TListValue _Lv;
		TMapFirstKeyValue _Fkv;
		TMapSecondKeyValue _Skv;

	};
}