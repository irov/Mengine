set( MY_LIB_NAME Math )

ADD_FILTER(
utils
	utils.h
)

ADD_FILTER(
convex8
    convex8.cpp
	convex8.h
    convex8_inline.h
)

ADD_FILTER(
rect
    rect.cpp
	rect.h
    rect_inline.h
)

ADD_FILTER(
uv4
    uv4.cpp
	uv4.h
    uv4_inline.h
)

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
clamp
	clamp.h
)

ADD_FILTER(
line2
	line2.cpp
	line2.h
	line2_inline.h
)

ADD_FILTER(
segment2
	segment2.cpp
	segment2.h
	segment2_inline.h
)

ADD_FILTER(
sphere2
	sphere2.cpp
	sphere2.h
	sphere2_inline.h
)


ADD_FILTER(
capsule2
	capsule2.cpp
	capsule2.h
	capsule2_inline.h
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
ccd
    ccd.cpp
    ccd.h
    ccd_inline.h
)

ADD_FILTER(
parabola
    parabola.cpp
    parabola.h
    parabola_inline.h
)


ADD_FILTER(
config
	config.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

