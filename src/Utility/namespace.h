#	pragma once

#define BEGIN_NAMESPACE(X) namespace X {
#define END_NAMESPACE() }

#define BEGIN_NAMESPACE_2(X0,X1)\
	namespace X0 {\
	namespace X1 {

#define END_NAMESPACE_2() }}

#define BEGIN_NAMESPACE_3(X0,X1,X2) \
	namespace X0 {\
	namespace X1 {\
	namespace X2 {

#define END_NAMESPACE_3() }}}

#define BEGIN_NAMESPACE_4(X0,X1,X2,X3) \
	namespace X0 {\
	namespace X1 {\
	namespace X2 {\
	namespace X3 {

#define END_NAMESPACE_4() }}}}