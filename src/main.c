#include <time.h>
#include "game.h"

/* known issues:
  - when the ball becomes supersonic, it phases through paddles
  - code is messy
  - some positions are not relative(meaning absolute)
*/

void reset(state_tt* p_state) {
  setup_game(p_state);
}

int main() {
  state_tt state;
  render_tt* p_render = &state.render;
  float vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f};
  uint32_t vert_shader, frag_shader;
  int success;

  success = init(&state, "./pp.vert.glsl", "./pp.frag.glsl");
  if (success != 0) {
    return success;
  }

  glGenVertexArrays(1, &p_render->vertex_array);
  glBindVertexArray(p_render->vertex_array);
  glGenBuffers(1, &p_render->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, p_render->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * p_render->render_count, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, p_render->vertex_comp_count, GL_FLOAT, GL_FALSE, p_render->vertex_comp_count * sizeof(float),
                        (void*)0);
  glEnableVertexAttribArray(0);
  glUseProgram(p_render->shader_program);
  glBindVertexArray(p_render->vertex_array);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  while (!glfwWindowShouldClose(p_render->window)) {
    render(&state);
    glfwPollEvents();

    if (!game_logic(&state)) {
      reset(&state);
      
      uint32_t old_time = time(0);
      while(old_time == time(0)) { render(&state); }
    }

    int code = glGetError();
    if (code != 0) {
      printf("GLerror occurred: %i\n", code);
    }
  }

  free(state.render.render_buffer);
  glfwTerminate();
  return 0;
}