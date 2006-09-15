#	pragma once
// This file contains:
// 1) type_id(type)
// 2) var_type_id(expersssion)
// 3) type_of(expression)

// IMPLEMENTATION
template<int ID>
class CTypeRegRoot
{
public:
	class id2type;
};

template<typename T, int ID>
class CTypeReg : public CTypeRegRoot<ID>
{
public:
	class CTypeRegRoot<ID>::id2type // This uses nice VC6-VC7 bugfeature
	{
	public:
		typedef T Type;
	};

	typedef void Dummy;
};

template<int N>
class CCounter;

// TUnused is required to force compiler to recompile CCountOf class
template<typename TUnused, int NTested = 0>
class CCountOf
{
public:
	enum
	{
		__if_exists(CCounter<NTested>) { count = CCountOf<TUnused, NTested + 1>::count }
		__if_not_exists(CCounter<NTested>) { count = NTested }
	};
};

template<class TTypeReg, class TUnused, int NValue> // Helper class
class CProvideCounterValue
{
public:
	enum { value = NValue };
};

// type_id
#define type_id(type) \
	(CProvideCounterValue< \
	/*register TYPE--ID*/ typename CTypeReg<type, CCountOf<type >::count>::Dummy, \
	/*increment compile-time Counter*/ CCounter<CCountOf<type >::count>, \
	/*pass value of Counter*/CCountOf<type >::count \
	>::value)

// Lets type_id() be > than 0
class __Increment_type_id { enum { value = type_id(__Increment_type_id) }; };

template<int NSize>
class sized
{
private:
	char m_pad[NSize];
};

template<typename T>
typename sized<type_id(T)> VarTypeID(T&);
template<typename T>
typename sized<type_id(const T)> VarTypeID(const T&);
template<typename T>
typename sized<type_id(volatile  T)> VarTypeID(volatile T&);
template<typename T>
typename sized<type_id(const volatile T)> VarTypeID(const volatile T&);

// Unfortunatelly, var_type_id() does not recognize references
#define var_type_id(var) \
	(sizeof(VarTypeID(var)))

// type_of
#define type_of(expression) \
	/* This uses nice VC6-VC7 bugfeature */ \
	CTypeRegRoot<var_type_id(expression)>::id2type::Type
