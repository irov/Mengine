#pragma once

#include "vec3.h"
#include "mat3.h"

void calculate_aabb_from_obb(mt::vec3f& min0, mt::vec3f& max0, const mt::vec2f& _pos, const mt::vec2f& _size,  const mt::mat3f& _wm);