/* (c)
Written by _Winnie, woowoowoow@bk.ru
*/

#	pragma once

#	ifdef __INTEL_COMPILER
#	pragma warning(push)
#	pragma warning(disable: 444) //destructor for base class "..." is not virtual
#	endif

namespace FOR_EACH_namespace
{
	struct WrapBase
	{
	};



	template <class Iterator>
	struct WrapIter: public WrapBase
	{
		WrapIter(const Iterator &it): it(it) {}
		mutable Iterator it;
	};

	template <class Iterator>
		WrapIter<Iterator> MakeWrapIter(const Iterator &it)
	{
		return WrapIter<Iterator>(it);
	}

	//Unwrap for STL containers
	template <class Container>
		typename Container::iterator &Unwrap(const WrapBase &iterator_wrap, Container &)
	{
		return static_cast<const WrapIter<typename Container::iterator> &>(iterator_wrap).it;
	}

	template <class Container>
		typename Container::const_iterator &Unwrap(const WrapBase &iterator_wrap, const Container &)
	{
		return static_cast<const WrapIter<typename Container::const_iterator> &>(iterator_wrap).it;
	}

	template <class Container>
		typename Container::reverse_iterator &RUnwrap(const WrapBase &iterator_wrap, Container &)
	{
		return static_cast<const WrapIter<typename Container::reverse_iterator> &>(iterator_wrap).it;
	}


	template <class Container>
		typename Container::const_reverse_iterator &RUnwrap(const WrapBase &iterator_wrap,const Container &)
	{
		return static_cast<const WrapIter<typename Container::const_reverse_iterator> &>(iterator_wrap).it;
	}

	//Unwrap for arrays (we do not need in const T specialization, cv-qualifers already are in T)
	template <class T, int size>
		T* &Unwrap(const WrapBase &iterator_wrap, T (&)[size])
	{
		return static_cast<const WrapIter<T*> &>(iterator_wrap).it;
	}

	//Begin/End for STL containers	template <class Container>
//	typename Container::iterator Begin(Container &c) { return c.begin(); }

	template <class Container>	typename Container::iterator Begin(Container &c) { return c.begin(); }
	template <class Container>	typename Container::iterator End(Container &c) { return c.end(); }
	template <class Container>	typename Container::const_iterator Begin(const Container &c) { return c.begin(); }
	template <class Container>	typename Container::const_iterator End(const Container &c) { return c.end(); }
	template <class Container>	typename Container::reverse_iterator RBegin(Container &c) { return c.rbegin(); }
	template <class Container>	typename Container::reverse_iterator REnd(Container &c) { return c.rend(); }
	template <class Container>	typename Container::const_reverse_iterator RBegin(const Container &c) { return c.rbegin(); }
	template <class Container>	typename Container::const_reverse_iterator REnd(const Container &c) { return c.rend(); }
	


    //Begin/End for arrays
	template <class T, int size>	T *Begin(T (&c)[size]) { return c; }
	template <class T, int size>	T *End(T (&c)[size]) { return c+size; }
} //namespace FOR_EACH_namespace

/*

Be carefull, do not use expressions as second argument `container` (it often repeated in
this macro).

const FOR_EACH_namespace::WrapBase\
&iterator_ = ...

.) non-const reference can't be initialized by temporary, so I use `const`
.) temporary, binded to reference live while reference is alive
.) `WrapBase iterator_` is const, but need in incrementing, => `mutable`
.) I can't write type of `container` ( like `typeof(container)`), so i need `static_cast`
trick and second parameter of `Unwrap(iterator_, container)`

if (int _fe_once_= 0) ;else \
for ( \

.) do not allow something like FOR_EACH(...) { ... } else {}
.) initialize counter as 0 for second `for`
( C++ don't allow `for (int i=0, double d=0; ...`
.) initialize `declaration` by (*iterator)
.) internal `for` used only for `declaration` initialization. It execute body only once.
.) IC++ 8.0, MSVC 7.1,  MSVC 6.0 can remove redudant jumps

examples:

1)
string strings[] = { "one", "two", "three" };
FOR_EACH(std::string &str, strings)
std::cout <<str <<std::endl;
2)

const std::vector<int> &v;
...
int s=1;
FOR_EACH(const int& i, v)
s*= i;
3)
std::vector<int> &v;
...
int s=1;
FOR_EACH(int& i, v)
{
s*= i;
i= 10;
}
*/

#define FOR_EACH(declaration, container)   \
	for (\
	const FOR_EACH_namespace::WrapBase\
	&iterator_ = FOR_EACH_namespace::MakeWrapIter(FOR_EACH_namespace::Begin(container)),\
	&iterator_end_ = FOR_EACH_namespace::MakeWrapIter(FOR_EACH_namespace::End(container));\
	\
	FOR_EACH_namespace::Unwrap(iterator_, (container)) != \
	FOR_EACH_namespace::Unwrap(iterator_end_, (container));\
	\
	++FOR_EACH_namespace::Unwrap(iterator_, (container))\
	) \
	if (int fe_once_= 0) ;else \
	for ( \
	declaration (*FOR_EACH_namespace::Unwrap(iterator_, (container))); \
	fe_once_ == 0; \
	++fe_once_)

#define FOR_EACH_REVERSE(declaration, container)   \
	for (\
	const FOR_EACH_namespace::WrapBase\
	&iterator_ = FOR_EACH_namespace::MakeWrapIter(FOR_EACH_namespace::RBegin(container)),\
	&iterator_end_ = FOR_EACH_namespace::MakeWrapIter(FOR_EACH_namespace::REnd(container));\
	\
	FOR_EACH_namespace::RUnwrap(iterator_, (container)) != \
	FOR_EACH_namespace::RUnwrap(iterator_end_, (container));\
	\
	++FOR_EACH_namespace::RUnwrap(iterator_, (container))\
	) \
	if (int fe_once_= 0) ;else \
	for ( \
	declaration (*FOR_EACH_namespace::RUnwrap(iterator_, (container))); \
	fe_once_ == 0; \
	++fe_once_)

#	ifdef __INTEL_COMPILER
#	pragma warning(pop)
#	endif
