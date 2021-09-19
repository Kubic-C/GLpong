#include "game.h"
#include <time.h>

float magic_speed = 0.02f;
float xspeedup = 1.2f;
float yspeedup = 1.1f;
bool xtreme_enabled = false;

void paddle_create(paddle_tt* p_paddle) {
  vec2_set(p_paddle->bl, 0.0f, 0.0f);
  vec2_set(p_paddle->br, 0.03f, 0.0f);
  vec2_set(p_paddle->tl, 0.0f, 0.3f);
  vec2_set(p_paddle->tr, 0.03f, 0.3f);
}

void check_for_xtreme(GLFWwindow* window) {
  if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !xtreme_enabled) {
    printf("XTREME MODE ENABLED!\n");
    xspeedup *= 2;
    yspeedup *= 2;
    magic_speed *= 2;
    xtreme_enabled = true;
  }
}

int program_create(uint32_t* p_program, const char* vert_dir, const char* frag_dir) {
  uint32_t vert_shader, frag_shader;

  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  if (compile_shader(vert_shader, vert_dir) != 0) {
    return -1;
  }

  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  if (compile_shader(frag_shader, frag_dir) != 0) {
    return -2;
  }

  *p_program = glCreateProgram();
  if (link_program(*p_program, vert_shader, frag_shader) != 0) {
    return -3;
  } else {
    glUseProgram(*p_program);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
  }

  return 0;
}

int gl_init(GLFWwindow** pp_window) {
  if (glfwInit() != GLFW_TRUE)
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  *pp_window = glfwCreateWindow(500, 500, "GLpong", NULL, NULL);
  if (*pp_window == NULL) {
    glfwTerminate();
    printf("failed to make GLFWwindow\n");
    return -2;
  }

  glfwMakeContextCurrent(*pp_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    printf("GLAD failed to init\n");
    return -3;
  }

  glViewport(0, 0, 500, 500);

  return 0;
}

void setup_game(state_tt* p_state) {
  {
    p_state->paddle_count = 2;
    paddle_create(&p_state->paddles[0]);
    vec2_set(p_state->paddles[0].pos, -0.8, 0.0f);

    paddle_create(&p_state->paddles[1]);
    vec2_set(p_state->paddles[1].pos, 0.8, 0.0f);
  }

  {
    float length;

    p_state->ball.size = 0.01f; // magic numbers...
    vec2_set(p_state->ball.center_pos, -p_state->ball.size, p_state->ball.size);
    vec2_set(p_state->ball.direction, randf(), randf());
    length = glm_vec2_norm(p_state->ball.direction);
    p_state->ball.direction[0] = p_state->ball.direction[0] / length;
    p_state->ball.direction[1] = p_state->ball.direction[1] / length;   
    glm_vec2_scale(p_state->ball.direction, magic_speed, p_state->ball.direction);
    if(fabs(p_state->ball.direction[0]) < fabs(p_state->ball.direction[1])) {
      float xcopy = p_state->ball.direction[0];
      // flip, so we dont bounce endlessly
      p_state->ball.direction[0] = p_state->ball.direction[1];
      p_state->ball.direction[1] = xcopy;
    }
  }
}

int init(state_tt* p_state, const char* vert_dir, const char* frag_dir) {\
  int ret;

  srand(time(0));
  ret = gl_init(&p_state->render.window);
  if (ret != 0) {
    return ret;
  }

  ret = program_create(&p_state->render.shader_program, vert_dir, frag_dir);
  if (ret != 0) {
    glfwTerminate();
    return ret;
  }

  {
    p_state->render.vertex_comp_count = 2;
    p_state->render.vertex_count = 6 + 6 + 6; // 6 repersents each quad (not using indexing, it would be 4 if so)
    p_state->render.render_count = p_state->render.vertex_count * p_state->render.vertex_comp_count; 
    p_state->render.render_buffer = malloc(p_state->render.render_count * sizeof(float));
  }

  setup_game(p_state);

  return 0;
}

void render(state_tt* p_state) {
  render_tt* p_render = &p_state->render;
  uint32_t render_count = p_render->render_count;
  float* render_buffer = p_render->render_buffer;

  // add paddles to all_vert. 6 is the amount of trianges in a quad without indexing
  uint32_t i = 0;
  uint32_t stride = 6 * 2;  // stride is the size of one rectangle
  for (; i < (p_state->paddle_count * stride); i += stride) {
    paddle_tt* p_paddle = p_state->paddles + i / stride;
    glm_vec2_add(p_paddle->bl, p_paddle->pos, &render_buffer[i + 0]);
    glm_vec2_add(p_paddle->br, p_paddle->pos, &render_buffer[i + 2]);
    glm_vec2_add(p_paddle->tl, p_paddle->pos, &render_buffer[i + 4]);
    // next triangle
    glm_vec2_add(p_paddle->tl, p_paddle->pos, &render_buffer[i + 6]);
    glm_vec2_add(p_paddle->tr, p_paddle->pos, &render_buffer[i + 8]);
    glm_vec2_add(p_paddle->br, p_paddle->pos, &render_buffer[i + 10]);
  }

  // now add the ball
  {
    ball_tt* ball = &p_state->ball;
    // this includes the position!
    vec2 bl_pos = { ball->center_pos[0] - ball->size, ball->center_pos[1] - ball->size};
    vec2 br_pos = { ball->center_pos[0] + ball->size, ball->center_pos[1] - ball->size};
    vec2 tl_pos = { ball->center_pos[0] - ball->size, ball->center_pos[1] + ball->size};
    vec2 tr_pos = { ball->center_pos[0] + ball->size, ball->center_pos[1] + ball->size};
    glm_vec2_copy(bl_pos, &render_buffer[i + 0]);
    glm_vec2_copy(br_pos, &render_buffer[i + 2]);
    glm_vec2_copy(tl_pos, &render_buffer[i + 4]);
    // next triangle
    glm_vec2_copy(tl_pos, &render_buffer[i + 6]);
    glm_vec2_copy(tr_pos, &render_buffer[i + 8]);
    glm_vec2_copy(br_pos, &render_buffer[i + 10]);
  }

  glUseProgram(p_render->shader_program);
  glBindVertexArray(p_render->vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, p_render->vertex_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * render_count, render_buffer);
  
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0,  p_render->vertex_count);

  glfwSwapBuffers(p_render->window);
  check_for_xtreme(p_render->window); // nothing to see here
}

uint8_t prevent_moving_outside(paddle_tt* p_paddle) {
  if (p_paddle->pos[1] + p_paddle->tr[1] > 1.0f) {
    p_paddle->pos[1] = 1.0f - p_paddle->tr[1];
    return true;
  }
  if (p_paddle->pos[1] + p_paddle->bl[1] < -1.0f) {
    p_paddle->pos[1] = -1.0f + p_paddle->bl[1];
    return true;
  }

  return false;
}

void update_paddle(GLFWwindow* window, paddle_tt* p_paddle, int down_key, int up_key) {
  if (glfwGetKey(window, down_key) == GLFW_PRESS) {
    p_paddle->pos[1] -= magic_speed;
  }
  if (glfwGetKey(window, up_key) == GLFW_PRESS) {
    p_paddle->pos[1] += magic_speed;
  }

  prevent_moving_outside(p_paddle);
}

bool update_ball(ball_tt* ball, uint32_t size, paddle_tt* paddles) {
  vec2 ball_coords[2];
  make_square(ball->center_pos, ball->size, ball_coords);

  // check for wall collision
  if (ball_coords[0][0] <= -1.0f) {  // LEFT
    ball->direction[0] = -ball->direction[0];
    ball->direction[0] *= magic_speed;
    // left is paddle[0] "player0"
    printf("player1 won!\n");
    return false;
  }
  if (ball_coords[1][0] >= 1.0f) { // RIGHT
    // lil' bit of boiler plate there and there
    ball->direction[0] = -ball->direction[0];
    ball->direction[0] *= magic_speed;
    // right is paddle[1] "player1"
    printf("player0 won!\n");
    return false;
  }
  if (ball_coords[0][1] <= -1.0f || ball_coords[1][1] >= 1.0f) {  // TOP & BOTTOM
    ball->direction[1] = -ball->direction[1];
  }

  // check for paddle collision
  bool no_coll = true;
  for (uint32_t i = 0; i < size; i++) {
    paddle_tt* paddle = &paddles[i];
    vec2 paddle_coords[2];    

    glm_vec2_add(paddle->pos, paddle->bl, paddle_coords[0]);
    glm_vec2_add(paddle->pos, paddle->tr, paddle_coords[1]);
    if (vec2_aabb(paddle_coords, ball_coords)) {
      uint8_t where;
      vec2 paddle_speed;
      float mag;

      glm_vec2_copy(ball->old_pos, ball->center_pos);
      make_square(ball->center_pos, ball->size, ball_coords);
      where = box2_where(paddle_coords, ball_coords);
      if(where == TOP_OR_BOTTOM) {
        ball->direction[0] = -(ball->direction[0] * xspeedup);
        ball->direction[1] = -(ball->direction[1] * yspeedup); 
      } else {
        ball->direction[0] = -(ball->direction[0] * xspeedup);
      }
      
      no_coll = false;
      break;
    }
  }

  if(no_coll == true) 
    glm_vec2_copy(ball->center_pos, ball->old_pos);

  glm_vec2_add(ball->center_pos, ball->direction, ball->center_pos);  // apply direction too ball

  return true;
}

bool game_logic(state_tt* p_state) {
  render_tt* p_render = &p_state->render;

  update_paddle(p_state->render.window, &p_state->paddles[0], GLFW_KEY_S, GLFW_KEY_W);
  update_paddle(p_state->render.window, &p_state->paddles[1], GLFW_KEY_DOWN, GLFW_KEY_UP);

  if (!update_ball(&p_state->ball, p_state->paddle_count, p_state->paddles)) {
    return false;
  }

  return true;
}