#pragma once

#include "vec3.h"
#include "mat3.h"

void calculate_aabb_from_obb(mt::vec2f& min0, mt::vec2f& max0, const mt::vec2f& _size, const mt::vec2f& _offset, const mt::mat3f& _wm);