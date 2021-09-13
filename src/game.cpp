#include "game.h"
#include "snake.h"
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include "SDL.h"
#include "food.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, std::size_t n_opp_snakes)
    : snake(grid_width, grid_height),
      grid_width(grid_width),
      grid_height(grid_height),
      n_opp_snakes(n_opp_snakes),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1))
{

  // initialize food vector
  for(int i = 0; i < 3 ; i++){  
    auto food_pos = findNewFoodPos();
    Food food(food_pos.first, food_pos.second);
    foods.push_back(food);
  }
}


std::pair <int,int> Game::findNewFoodPos(){
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x,y)) {
      return std::make_pair(x,y);
    }
  }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  // Simulate opponent snake threads

  // Create OppSnake instances with shared pointer.
  for(size_t ns = 0; ns < n_opp_snakes; ns++){
    opp_snakes.push_back(std::make_shared<OppSnake>(grid_width, grid_height, ns));
  }
  // Initialize opponent snake threads 
  std::for_each(opp_snakes.begin(), opp_snakes.end(), [](std::shared_ptr<OppSnake> &s){
    s->simulate();
  });

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, foods, opp_snakes);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

/*
void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}
*/

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  // Get OppSnake New Positions.
  std::for_each(opp_snakes.begin(), opp_snakes.end(), [](std::shared_ptr<OppSnake> &s){
    auto ftrOppSnake = std::async(&OppSnake::updatePos, s);
    auto result = ftrOppSnake.get();
    std::cout << "OppSnake id: "<< s->getId() << " - result: " << result << std::endl;
  });

  int new_US_x = static_cast<int>(snake.head_x);
  int new_US_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  std::cout << "size of foods: "<< foods.size() << std::endl;

  
  for(auto food : foods){
    if (food.getX() == new_US_x && food.getY() == new_US_y) {
      score++;
      auto food_pos = findNewFoodPos();
      food.PlaceFood(food_pos.first, food_pos.second);
      // Grow snake and increase speed.
      snake.GrowBody();
      snake.speed += 0.02;
    }
  }
  
  
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

