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
    : _snake(grid_width, grid_height),
      _grid_width(grid_width),
      _grid_height(grid_height),
      _n_opp_snakes(n_opp_snakes),
      _engine(_dev()),
      _random_w(0, static_cast<int>(grid_width - 1)),
      _random_h(0, static_cast<int>(grid_height - 1)),
      _random_d(0,3)
{
  // Initialize food vector
  for(int i = 0; i < 3 ; i++){  
    auto food_pos = findNewFoodPos();
    Food food(std::move(food_pos.first), std::move(food_pos.second));
    _foods.push_back(food);
  }

  // Initialize ImmortalFood
  _i_food = std::make_shared<Food>();
  PlaceIFood();

  // Initialize ImmortalMode
  _i_mode = std::make_shared<ImmortalMode>(_i_food);
}

// Place ImmortalFood
void Game::PlaceIFood(){
  int x, y;
  while(true){
    x = _random_w(_engine);
    y = _random_h(_engine);

    if (!_snake.SnakeCell(x,y)){
      _i_food->PlaceFood(x,y);
      return;
    }
  }
}

// Find a random new position on the grid for new food
std::pair <int,int> Game::findNewFoodPos(){
  int x, y;
  while (true) {
    x = _random_w(_engine);
    y = _random_h(_engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!_snake.SnakeCell(x,y)) {
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
  for(size_t ns = 0; ns < _n_opp_snakes; ns++){
    _opp_snakes.push_back(std::make_shared<OppSnake>(_grid_width, _grid_height, ns));
  }

  // Initialize ImmortalMode Thread
  std::thread t(&ImmortalMode::simulate, _i_mode);

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, _snake);
    Update();
    renderer.Render(_snake, _opp_snakes, _foods, _i_food, _i_mode->is_activated());

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(_score, frame_count);
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
  if (!_snake.alive) return;

  _snake.Update();

  int new_US_x = static_cast<int>(_snake.head_x);
  int new_US_y = static_cast<int>(_snake.head_y);

  // Initialize crash_bool_list with all true
  std::vector<bool> crash_bool_list;
  for(int i = 0; i < _opp_snakes.size(); i++){
    crash_bool_list.push_back(true);
  }
  
  // Update OppSnake positions.
  int ind = 0;
  for (auto &opp_snake : _opp_snakes){
    int random_direction = _random_d(_engine);
    auto ftrOppSnake = std::async(&OppSnake::Update, opp_snake, std::move(random_direction), new_US_x, new_US_y);
    crash_bool_list[ind] = ftrOppSnake.get();
    ind++;
  }
  
  // Check if there's food at UserSnake's position
  for(auto &food : _foods){
    if (food.getX() == new_US_x && food.getY() == new_US_y) {
      _score++;
      auto food_pos = findNewFoodPos();
      food.PlaceFood(std::move(food_pos.first), std::move(food_pos.second));

      // Grow snake and increase speed.
      _snake.GrowBody();
      _snake.speed += 0.02;

      // Grow opp_snakes and increase speed.
      for (auto &opp_snake : _opp_snakes){
        opp_snake->GrowBody();
        opp_snake->speed += 0.02;
      }
    }
  }

  // Check if UserSnake crashed into any of the OppSnakes.
  _snake.alive = std::all_of(crash_bool_list.begin(), crash_bool_list.end(), [](bool crash){return crash;});

  // If Immortal Mode is activated, the UserSnake won't die even if it touched other snakes.
  if(_snake.alive == false && _i_mode->is_activated() == true){
    _snake.alive = true;
  }

  // Activate Immortal Mode.
  if (_i_food->getX() == new_US_x && _i_food->getY() == new_US_y){
    auto food_pos = findNewFoodPos();
    _i_mode->activate(std::move(food_pos));
    _i_food->PlaceFood(-1, -1);
  }
}

int Game::GetScore() const { return _score; }
int Game::GetSize() const { return _snake.size; }

std::mutex ImmortalMode::_MI_mtx;

// Constructor
ImmortalMode::ImmortalMode(std::shared_ptr<Food> i_food) : _activated(false),
                                                           _duration_sec(5.0),
                                                           _i_food(i_food),
                                                           _ftr_i_food_pos(std::make_pair(0,0)) {}


void ImmortalMode::simulate(){
  auto last_updated = std::chrono::system_clock::now();

  // Use mutex lock to safely access _activate.
  std::unique_lock<std::mutex> lck(_MI_mtx);
  lck.unlock();
  while(true){
    // sleep for 1 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    lck.lock();

    // If _activated becomes true, wait for 5 seconds and place the ImmortalFood again
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

// Lock and return _activated
bool ImmortalMode::is_activated(){
  std::unique_lock<std::mutex> lck(_MI_mtx);
  return _activated;
}

// User Snake ate the ImmortalFood. Activate immortal mode.
// Also, input the next position of the new ImmortalFood
void ImmortalMode::activate(std::pair <int,int> new_i_food_pos){
  std::unique_lock<std::mutex> lck(_MI_mtx);
  _activated = true;
  _ftr_i_food_pos = new_i_food_pos;
}
