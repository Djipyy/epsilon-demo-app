#include <eadk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define nb_bricks_orig 10

const char eadk_app_name[]
#if PLATFORM_DEVICE
__attribute__((section(".rodata.eadk_app_name")))
#endif
= "App";

const uint32_t eadk_api_level
#if PLATFORM_DEVICE
__attribute__((section(".rodata.eadk_api_level")))
#endif
= 0;

eadk_color_t random_color()
{
  return (eadk_color_t)eadk_random();
}

eadk_rect_t random_screen_rect()
{
  uint16_t x = eadk_random() % (EADK_SCREEN_WIDTH - 1);
  uint16_t y = eadk_random() % (EADK_SCREEN_HEIGHT - 1);
  uint16_t width = eadk_random() % (EADK_SCREEN_WIDTH - x);
  uint16_t height = eadk_random() % (EADK_SCREEN_HEIGHT - y);
  return (eadk_rect_t) { x, y, width, height };
}

// void draw_random_colorful_rectangles()
// {
//   eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
//   for (int i = 0; i < 100; i++) {
//     eadk_display_push_rect_uniform(random_screen_rect(), random_color());
//   }
// }

void draw_colored_background(eadk_color_t color)
{

  eadk_rect_t screen_rect = { 0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT };
  eadk_display_push_rect_uniform(screen_rect, color);
}

void draw_ball(uint16_t ball_x, uint16_t ball_y, uint16_t ball_size)
{
  // Check if ball is in bound and correct if necessary
  if (ball_x < 0) {
    ball_x = 0;
  }
  if (ball_x > EADK_SCREEN_WIDTH - ball_size) {
    ball_x = EADK_SCREEN_WIDTH - ball_size;
  }
  if (ball_y < 0) {
    ball_y = 0;
  }
  if (ball_y > EADK_SCREEN_HEIGHT - ball_size) {
    ball_y = EADK_SCREEN_HEIGHT - ball_size;
  }

  eadk_rect_t ball = { ball_x, ball_y, ball_size, ball_size };
  eadk_display_push_rect_uniform(ball, eadk_color_black);
}

void draw_brick(uint16_t brick_x, uint16_t brick_y, uint16_t brick_width, uint16_t brick_height)
{
  eadk_rect_t brick = { brick_x, brick_y, brick_width, brick_height };
  eadk_display_push_rect_uniform(brick, eadk_color_black);
}


eadk_rect_t make_small_random_rect()
{
  // The rect must be at least in the upper two thirds of the screen
  uint16_t x = eadk_random() % (EADK_SCREEN_WIDTH - 1);
  uint16_t y = eadk_random() % (EADK_SCREEN_HEIGHT / 3);
  uint16_t width = eadk_random() % (8) + 10;
  uint16_t height = eadk_random() % (8) + 10;
  printf("x: %d, y: %d, width: %d, height: %d\n", x, y, width, height);
  return (eadk_rect_t) { x, y, width, height };
}

void event_loop()
{

  uint32_t score = 0;
  char score_s[20];
  uint16_t ball_x = EADK_SCREEN_WIDTH / 2;
  uint16_t ball_y = EADK_SCREEN_HEIGHT / 2;
  uint16_t  ball_vx = 1;
  uint16_t ball_vy = 1;
  uint16_t ball_size = 20;

  uint16_t platform_width = 40;
  uint16_t platform_x = EADK_SCREEN_WIDTH / 2 - platform_width / 2;
  uint16_t platform_y = EADK_SCREEN_HEIGHT - 10;

  eadk_rect_t bricks[nb_bricks_orig];
  for (size_t i = 0; i < nb_bricks_orig; i++) {
    bricks[i] = make_small_random_rect();
  }

  while (true) {

    eadk_keyboard_state_t keyboard = eadk_keyboard_scan();
    if (eadk_keyboard_key_down(keyboard, eadk_key_home)) {
      ball_x = EADK_SCREEN_WIDTH / 2;
      ball_y = EADK_SCREEN_HEIGHT / 2;
      ball_vy = 1;
      ball_vx = 1;
      platform_x = EADK_SCREEN_WIDTH / 2 - platform_width / 2;
      platform_y = EADK_SCREEN_HEIGHT - 10;
    }



    ball_x += ball_vx;
    ball_y += ball_vy;

    // printf("Ball x: %d, Ball y: %d, Score : %d\n", ball_x, ball_y, score);

    if (ball_y > EADK_SCREEN_HEIGHT - ball_size) {
      ball_vx = 0;
      ball_vy = 0;
    }
    if (ball_x < 0 || ball_x > EADK_SCREEN_WIDTH - ball_size) {
      ball_vx = -ball_vx;
    }
    if (ball_y < 0 || ball_y > EADK_SCREEN_HEIGHT - ball_size) {
      ball_vy = -ball_vy;
    }
    if (ball_y + ball_size >= platform_y && ball_x + ball_size >= platform_x && ball_x <= platform_x + platform_width) {
      ball_vy = -ball_vy;
    }

    draw_colored_background(eadk_color_white);

    eadk_display_push_rect_uniform((eadk_rect_t) { platform_x, platform_y, platform_width, 5 }, eadk_color_black);
    draw_ball(ball_x, ball_y, ball_size);

    for (size_t i = 0; i < nb_bricks_orig; i++) {
      draw_brick(bricks[i].x, bricks[i].y, bricks[i].width, bricks[i].height);
    }

    for (size_t i = 0; i < nb_bricks_orig; i++) {
      if (ball_x + ball_size >= bricks[i].x && ball_x <= bricks[i].x + bricks[i].width && ball_y + ball_size >= bricks[i].y && ball_y <= bricks[i].y + bricks[i].height) {
        bricks[i] = make_small_random_rect();
        ball_vx = -ball_vx;
        ball_vy = -ball_vy;
        score += 1;
      }
    }

    if (eadk_keyboard_key_down(keyboard, eadk_key_right) && platform_x < EADK_SCREEN_WIDTH - platform_width) {
      platform_x += 1;
    }
    if (eadk_keyboard_key_down(keyboard, eadk_key_left) && platform_x > platform_width) {
      platform_x -= 1;
    }


    sprintf(score_s, "%d", score);
    eadk_display_draw_string(score_s, (eadk_point_t) { 0, 0 }, true, eadk_color_black, eadk_color_white);

  }

}

int main(int argc, char* argv[])
{
  printf("External data : '%s'\n", eadk_external_data);
  eadk_timing_msleep(3000);
  // draw_random_colorful_rectangles();
  // draw_colored_background(eadk_color_white);
  // draw_random_buffer();
  // eadk_display_draw_string("Hello, world!", (eadk_point_t) { 0, 0 }, true, eadk_color_black, eadk_color_white);
  event_loop();
}

