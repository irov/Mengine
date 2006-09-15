#	pragma once

template<class T>	inline const char * get_info_scanf(){return "%g";};
template<>			inline const char * get_info_scanf<bool>(){return "%c";};
template<>			inline const char * get_info_scanf<int>(){return "%d";};
template<>			inline const char * get_info_scanf<float>(){return "%f";};
template<>			inline const char * get_info_scanf<double>(){return "%lf";};

template< class T >
inline void tsscanf(const char *str,T &value)
{
	sscanf(str,get_info_scanf<T>(),&value);
}

inline void tsscanf(const char *str,const char *value)
{
	sscanf(str,"%s",value);
}

template< class T >
inline void tsprintf(char *str,T value)
{
	sprintf(str,get_info_scanf<T>(),value);
}

inline void tsprintf(char *str,const char *value)
{
	sprintf(str,"%s",value);
}