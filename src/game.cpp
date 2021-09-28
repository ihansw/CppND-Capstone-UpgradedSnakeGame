#include "game.h"
#include "snake.h"
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include "SDL.h"
#include "food.h"
// #include <memory>

Game::Game(std::size_t grid_width, std::size_t grid_height, std::size_t n_opp_snakes)
    : snake(grid_width, grid_height),
      grid_width(grid_width),
      grid_height(grid_height),
      n_opp_snakes(n_opp_snakes),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      random_d(0,3)
{
  // initialize food vector
  for(int i = 0; i < 3 ; i++){  
    auto food_pos = findNewFoodPos();
    Food food(food_pos.first, food_pos.second);
    foods.push_back(food);
  }

  // initialize ImmortalFood
  i_food = std::make_shared<Food>();
  PlaceIFood();

  // initialize ImmortalMode
  i_mode = std::make_shared<ImmortalMode>(i_food);
}

void Game::PlaceIFood(){
  int x, y;
  while(true){
    x = random_w(engine);
    y = random_h(engine);

    if (!snake.SnakeCell(x,y)){
      i_food->PlaceFood(x,y);
      return;
    }
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

  // Create OppSnake instances with shared pointer.
  for(size_t ns = 0; ns < n_opp_snakes; ns++){
    opp_snakes.push_back(std::make_shared<OppSnake>(grid_width, grid_height, ns));
  }

  // Initialize ImmortalMode Thread
  std::thread t(&ImmortalMode::simulate, i_mode);

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, opp_snakes, foods, i_food, i_mode->is_activated());

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

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_US_x = static_cast<int>(snake.head_x);
  int new_US_y = static_cast<int>(snake.head_y);

  // initialize crash_bool_list with all true
  std::vector<bool> crash_bool_list;
  for(int i = 0; i < opp_snakes.size(); i++){
    crash_bool_list.push_back(true);
  }
  
  // Update OppSnake positions.
  int ind = 0;
  for (auto &opp_snake : opp_snakes){
    int random_direction = random_d(engine);
    auto ftrOppSnake = std::async(&OppSnake::Update, opp_snake, random_direction, new_US_x, new_US_y);
    crash_bool_list[ind] = ftrOppSnake.get();

    ind++;
  }
  
  // Check if there's food over here
  for(auto &food : foods){
    if (food.getX() == new_US_x && food.getY() == new_US_y) {
      score++;
      auto food_pos = findNewFoodPos();
      food.PlaceFood(food_pos.first, food_pos.second);

      // Grow snake and increase speed.
      snake.GrowBody();
      snake.speed += 0.02;

      // Grow opp_snakes and increase speed.
      for (auto &opp_snake : opp_snakes){
        opp_snake->GrowBody();
        opp_snake->speed += 0.02;
      }
    }
  }

  // Check if UserSnake crashed into any of the OppSnakes.
  snake.alive = std::all_of(crash_bool_list.begin(), crash_bool_list.end(), [](bool crash){return crash;});

  // If Immortal Mode is activated, the user snake won't die even if it touched other snakes.
  if(snake.alive == false && i_mode->is_activated() == true){
    snake.alive = true;
  }

  // Activate Immortal Mode.
  if (i_food->getX() == new_US_x && i_food->getY() == new_US_y){
    auto food_pos = findNewFoodPos();
    i_mode->activate(food_pos);
    i_food->PlaceFood(-1, -1);
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

std::mutex ImmortalMode::_MI_mtx;

// Constructor
ImmortalMode::ImmortalMode(std::shared_ptr<Food> i_food) : _activated(false),
                                                           _duration_sec(5.0),
                                                           _i_food(i_food),
                                                           _ftr_i_food_pos(std::make_pair(0,0)) {}


void ImmortalMode::simulate(){
  auto last_updated = std::chrono::system_clock::now();
  std::unique_lock<std::mutex> lck(_MI_mtx);
  lck.unlock();
  while(true){
    // sleep for 1 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    lck.lock();
    if(_activated == true){
      std::cout << "Immortal Mode activated!" << std::endl;
      lck.unlock();

      last_updated = std::chrono::system_clock::now();

      while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_updated).count();

        // if 5 seconds has passed, break out of the loop
        if (time_elapsed > _duration_sec){
          break;
        }
      }
      
      lck.lock();
      std::cout << "ImmortalMode released" << std::endl;
      _activated = false;
      _i_food->PlaceFood(_ftr_i_food_pos.first, _ftr_i_food_pos.second);
    }
    lck.unlock();    
  }
}

bool ImmortalMode::is_activated(){
  // lock and return _activated
  std::unique_lock<std::mutex> lck(_MI_mtx);
  return _activated;
}

// User Snake ate the ImmortalFood. Activate immortal mode.
void ImmortalMode::activate(std::pair <int,int> new_i_food_pos){
    // lock and update the _activated
  std::unique_lock<std::mutex> lck(_MI_mtx);
  _activated = true;
  _ftr_i_food_pos = new_i_food_pos;
}
