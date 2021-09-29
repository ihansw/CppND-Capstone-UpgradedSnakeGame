#include "renderer.h"
#include "food.h"
#include <iostream>
#include <string>
#include <algorithm>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : _screen_width(screen_width),
      _screen_height(screen_height),
      _grid_width(grid_width),
      _grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  _sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, _screen_width,
                                _screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == _sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  _sdl_renderer = SDL_CreateRenderer(_sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == _sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(_sdl_window);
  SDL_Quit();
}

void Renderer::Render(Snake const snake, std::vector<std::shared_ptr<OppSnake>> opp_snakes, 
                      std::vector<Food> foods, std::shared_ptr<Food> i_food,
                      bool i_mode_activated) {
  SDL_Rect block;
  block.w = _screen_width / _grid_width;
  block.h = _screen_height / _grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(_sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(_sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(_sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  for (Food food : foods){
    block.x = food.getX() * block.w;
    block.y = food.getY() * block.h;
    SDL_RenderFillRect(_sdl_renderer, &block);
  }

  // Render Immortal Food
  SDL_SetRenderDrawColor(_sdl_renderer, 0x00, 0xE6, 0xE6, 0xFA);
  block.x = i_food->getX() * block.w;
  block.y = i_food->getY() * block.h;
  SDL_RenderFillRect(_sdl_renderer, &block);

  // Render OppSnakes's body
  for(auto opp_snake : opp_snakes){
    for (SDL_Point const &point : opp_snake->body) {
      SDL_SetRenderDrawColor(_sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      block.x = point.x * block.w;
      block.y = point.y * block.h;
      SDL_RenderFillRect(_sdl_renderer, &block);
  }

  // Render OppSnakes' heads
  for(auto opp_snake : opp_snakes){
    SDL_SetRenderDrawColor(_sdl_renderer, 0xFF, 0x00, 0xFF, 0x00);
    block.x = static_cast<int>(opp_snake->head_x) * block.w;
    block.y = static_cast<int>(opp_snake->head_y) * block.h;
    SDL_RenderFillRect(_sdl_renderer, &block);
  }

  // Render User Snake's body
  if (i_mode_activated){
    SDL_SetRenderDrawColor(_sdl_renderer, 0x00, 0xFF, 0x00, 0x00);
  } else {
    SDL_SetRenderDrawColor(_sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  }
  
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(_sdl_renderer, &block);
  }

  // Render User Snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (i_mode_activated){
    SDL_SetRenderDrawColor(_sdl_renderer, 0x00, 0x00, 0xFF, 0x00);
  }
  else if (snake.alive) {
    SDL_SetRenderDrawColor(_sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(_sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(_sdl_renderer, &block);

  

  };

  // Update Screen
  SDL_RenderPresent(_sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(_sdl_window, title.c_str());
}
