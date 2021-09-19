add_executable(pong.out 
  "main.c" "base.h" 
  "utility.h" "utility.c" 
  "game.h" "game.c")

target_link_libraries(pong.out PUBLIC glfw glad cglm)
