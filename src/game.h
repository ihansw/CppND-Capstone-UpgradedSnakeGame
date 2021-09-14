#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include <thread>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "food.h"


class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, std::size_t n_opp_snakes);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  std::pair <int,int> findNewFoodPos();

 private:
  Snake snake;
  std::vector<std::shared_ptr<OppSnake>> opp_snakes;
  int n_opp_snakes;
  // SDL_Point food;
  std::vector<Food> foods;

  int grid_width;
  int grid_height;

  int score{0};

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_d;

  //void PlaceFood();
  void Update();
  
};


#endif