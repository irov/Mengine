#	pragma once

template<class T>
class pool_vector
{
public:
	pool_vector(size_t size)
	{
		node_vector = new T[size];
		dead_node_index = new size_t[size];

		node_size = 0;
		dead_node_size = 0;
	}

	~pool_vector()
	{
		delete node_vector;
		delete dead_node_index;
	}

public:

	T * allocate()
	{
		if( dead_node_size == 0 )
		{
			T * ret = node_vector + node_size;
			++node_size;
			return ret;
		}
		else
		{
			--dead_node_size;
			size_t index = dead_node_index[dead_node_size];
			T * ret = node_vector + index;
			return ret;
		}
	}

	T * construct()
	{
		T *ret = allocate();
		//ret->__ctor();
		new (ret) T();
		return ret;
	}

	void free(const T *t)
	{
		size_t offset = sizeof(T);
		size_t index = size_t(t - node_vector) / offset;
		dead_node_index[dead_node_size] = index;
		++dead_node_size;
	}

	size_t size()
	{
		return node_size - dead_node_size;
	}

	bool empty()
	{
		return node_size == dead_node_size;
	}	

private:
	T *node_vector;
	size_t *dead_node_index;

	size_t node_size;
	size_t dead_node_size;
};