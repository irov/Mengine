CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/box2d)

project(box2d)

SET(LIB_TYPE STATIC)

set(BOX2D_SOURCE_FILES
	${SOURCE_DIR}/src/collision/b2_broad_phase.cpp
	${SOURCE_DIR}/src/collision/b2_chain_shape.cpp
	${SOURCE_DIR}/src/collision/b2_circle_shape.cpp
	${SOURCE_DIR}/src/collision/b2_collide_circle.cpp
	${SOURCE_DIR}/src/collision/b2_collide_edge.cpp
	${SOURCE_DIR}/src/collision/b2_collide_polygon.cpp
	${SOURCE_DIR}/src/collision/b2_collision.cpp
	${SOURCE_DIR}/src/collision/b2_distance.cpp
	${SOURCE_DIR}/src/collision/b2_dynamic_tree.cpp
	${SOURCE_DIR}/src/collision/b2_edge_shape.cpp
	${SOURCE_DIR}/src/collision/b2_polygon_shape.cpp
	${SOURCE_DIR}/src/collision/b2_time_of_impact.cpp
	${SOURCE_DIR}/src/common/b2_block_allocator.cpp
	${SOURCE_DIR}/src/common/b2_draw.cpp
	${SOURCE_DIR}/src/common/b2_math.cpp
	${SOURCE_DIR}/src/common/b2_settings.cpp
	${SOURCE_DIR}/src/common/b2_stack_allocator.cpp
	${SOURCE_DIR}/src/common/b2_timer.cpp
	${SOURCE_DIR}/src/dynamics/b2_body.cpp
	${SOURCE_DIR}/src/dynamics/b2_chain_circle_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_chain_circle_contact.h
	${SOURCE_DIR}/src/dynamics/b2_chain_polygon_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_chain_polygon_contact.h
	${SOURCE_DIR}/src/dynamics/b2_circle_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_circle_contact.h
	${SOURCE_DIR}/src/dynamics/b2_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_contact_manager.cpp
	${SOURCE_DIR}/src/dynamics/b2_contact_solver.cpp
	${SOURCE_DIR}/src/dynamics/b2_contact_solver.h
	${SOURCE_DIR}/src/dynamics/b2_distance_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_edge_circle_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_edge_circle_contact.h
	${SOURCE_DIR}/src/dynamics/b2_edge_polygon_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_edge_polygon_contact.h
	${SOURCE_DIR}/src/dynamics/b2_fixture.cpp
	${SOURCE_DIR}/src/dynamics/b2_friction_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_gear_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_island.cpp
	${SOURCE_DIR}/src/dynamics/b2_island.h
	${SOURCE_DIR}/src/dynamics/b2_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_motor_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_mouse_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_polygon_circle_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_polygon_circle_contact.h
	${SOURCE_DIR}/src/dynamics/b2_polygon_contact.cpp
	${SOURCE_DIR}/src/dynamics/b2_polygon_contact.h
	${SOURCE_DIR}/src/dynamics/b2_prismatic_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_pulley_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_revolute_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_rope_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_weld_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_wheel_joint.cpp
	${SOURCE_DIR}/src/dynamics/b2_world.cpp
	${SOURCE_DIR}/src/dynamics/b2_world_callbacks.cpp
	${SOURCE_DIR}/src/rope/b2_rope.cpp)
    
include_directories(${SOURCE_DIR}/include)
include_directories(${SOURCE_DIR}/src)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${BOX2D_SOURCE_FILES})