
set( MY_LIB_NAME Math )

ADD_FILTER(
angle
	angle.cpp
	angle.h
	angle_inline.h
)

ADD_FILTER(
box2
	box2.cpp
	box2.h
	box2_inline.h
)

ADD_FILTER(
box3
	box3.cpp
	box3.h
	box3_inline.h
)

ADD_FILTER(
convex_poly
	convexpoly2.cpp
	convexpoly2.h
	convexpoly2_inline.h
)

ADD_FILTER(
clamp
	clamp.h
)

ADD_FILTER(
line
	line.cpp
	line.h
	line_inline.h
)

ADD_FILTER(
triangle
	triangle.cpp
	triangle.h
	triangle_inline.h
)

ADD_FILTER(
mat2
	mat2.cpp
	mat2.h
	mat2_inline.h
)

ADD_FILTER(
mat3
	mat3.cpp
	mat3.h
	mat3_inline.h
)

ADD_FILTER(
mat4
	mat4.cpp
	mat4.h
	mat4_inline.h
)

ADD_FILTER(
plane
	plane.cpp
	plane.h
	plane_inline.h
)

ADD_FILTER(
quat
	quat.cpp
	quat.h
	quat_inline.h
)

ADD_FILTER(
rand
	rand.h
)

ADD_FILTER(
simplex
	simplex.cpp
	simplex.h
	simplex_inline.h
)

ADD_FILTER(
vec2
	vec2.cpp
	vec2.h
	vec2_inline.h
)

ADD_FILTER(
vec3
	vec3.cpp
	vec3.h
	vec3_inline.h
)

ADD_FILTER(
vec4
	vec4.cpp
	vec4.h
	vec4_inline.h
)

ADD_FILTER(
config
	config.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

