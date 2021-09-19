#ifndef _PING_PONG_GAME_H_INCLUDED
#define _PING_PONG_GAME_H_INCLUDED

#include "base.h"
#include "utility.h"

typedef struct {
  // bottom left, bottom right, top left, top right
  vec2 bl, br, tl, tr, pos;
} paddle_tt;

typedef struct {
  vec2 direction;
  vec2 center_pos;
  vec2 old_pos;
  float size;
} ball_tt;

typedef struct {
  uint32_t vertex_array;
  uint32_t vertex_buffer;
  uint32_t index_buffer;
  uint32_t shader_program;
  uint32_t vertex_count;
  uint32_t vertex_comp_count;
  uint32_t render_count;
  float* render_buffer;
  GLFWwindow* window;
} render_tt;

typedef struct {
  uint32_t paddle_count;
  // first paddle is the player
  paddle_tt paddles[2];
  ball_tt ball;
  render_tt render;
} state_tt;

int init(state_tt* p_state, const char* vert_dir, const char* frag_dir);
void setup_game(state_tt* p_state);
void render(state_tt* p_state);
uint8_t prevent_moving_outside(paddle_tt* p_paddle);
bool update_ball(ball_tt* ball, uint32_t size, paddle_tt* paddles);
bool game_logic(state_tt* p_state);

#endif  // _PING_PONG_GAME_H_INCLUDED