#include "utility.h"
#include "base.h"

enum { x = 0, y = 1, z = 2 };

enum {
  bl = 0,
  tr = 1
};

int compile_shader(uint32_t shader_id, const char* dir) {
  int src_size;
  FILE* file_src;
  char* local_src;
  char info_log[512];
  int success;

  file_src = fopen(dir, "r");
  if (file_src == NULL) {
    return -1;
  }

  fseek(file_src, 0, SEEK_END);
  src_size = ftell(file_src);
  fseek(file_src, 0, SEEK_SET);
  local_src = malloc(sizeof(char) * src_size);
  fread(local_src, sizeof(char), src_size, file_src);
  fclose(file_src);

  glShaderSource(shader_id, 1, (const char* const*)&local_src, &src_size);
  glCompileShader(shader_id);
  free(local_src);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, 512, NULL, info_log);
    printf("shader compile failure: %s", info_log);
    return -1;
  }

  return 0;
}

int link_program(uint32_t program, uint32_t vert_shader, uint32_t frag_shader) {
  char info_log[512];
  int success;

  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    printf("program link failure: %s", info_log);
    return -1;
  }

  return 0;
}

uint8_t vec2_aabb(vec2 b1[2], vec2 b2[2]) {
  return (b1[0][0] <= b2[1][0] && b1[1][0] >= b2[0][0]) 
      && (b1[0][1] <= b2[1][1] && b1[1][1] >= b2[0][1]);
}

uint8_t box2_where(vec2 b1[2], vec2 b2[2]) {
  if(b1[0][0] <= b2[1][0] && b1[1][0] >= b2[0][0]) {
    if(b1[0][1] >= b2[1][1] || b1[1][1] <= b2[0][1]) {
      return TOP_OR_BOTTOM;
    }
  } else {
    return LEFT_OR_RIGHT;
  }

  return 0;
}

void make_square(vec2 center_pos, float size, vec2 box[2]) {
  box[0][0] = center_pos[0] - size;
  box[0][1] = center_pos[1] - size;
  box[1][0] = center_pos[0] + size;
  box[1][1] = center_pos[1] + size;
}