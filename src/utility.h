#ifndef _PING_PONG_UTILITY_H_INCLUDED
#define _PING_PONG_UTILITY_H_INCLUDED

#include "base.h"

#define NO_COLL 0
#define TOP_OR_BOTTOM 1
#define LEFT_OR_RIGHT 2

int compile_shader(uint32_t shader_id, const char* dir);
int link_program(uint32_t program, uint32_t vert_shader, uint32_t frag_shader);
uint8_t vec2_aabb(vec2 b1[2], vec2 b2[2]);
uint8_t box2_where(vec2 b1[2], vec2 b2[2]);
void make_square(vec2 center_pos, float size, vec2 box[2]);

CGLM_INLINE float randf() {
  return sin(rand());
}

CGLM_INLINE void vec2_set(vec2 v, float x, float y) {
  v[0] = x;
  v[1] = y;
}

#endif // _PING_PONG_UTILITY_H_INCLUDED