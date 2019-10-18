SET(MY_LIB_NAME Config)

ADD_FILTER(
src
    Array.h
    ArrayString.h
    Blobject.h
    Char.h
    DynamicArray.h
    String.h
    StringRegex.h
    VectorString.h
    Stringstream.h
    Typedef.h
    Floats.h
    Vector.h
    Deque.h
    Map.h
    Pair.h
    UnorderedMap.h
    List.h
    Tuple.h
    Regex.h
    Atomic.h
    Lambda.h
    Define.h
	Config.h
    Export.h
	Secure.h
    StdIO.h
    Metaprogramming.h
	cmake_dummy.cpp
)

ADD_MENGINE_LIBRARY()