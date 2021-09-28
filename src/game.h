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

class ImmortalMode{
  public:
    ImmortalMode(std::shared_ptr<Food> i_food);
    
    void activate(std::pair <int,int> new_i_food_pos);
    bool is_activated();
    void simulate();
    void placeIFoodLater(Food &i_food);

  private:
    std::pair <int,int> _ftr_i_food_pos;
    std::shared_ptr<Food> _i_food;
    static std::mutex _MI_mtx;
    bool _activated; 
    float _duration_sec;
};

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, std::size_t n_opp_snakes);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  std::pair <int,int> findNewFoodPos();
  std::shared_ptr<ImmortalMode> i_mode;

 private:
  Snake snake;
  std::vector<std::shared_ptr<OppSnake>> opp_snakes;
  int n_opp_snakes;
  std::vector<Food> foods;
  std::shared_ptr<Food> i_food;
  
  int grid_width;
  int grid_height;

  int score{0};

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_d;

  void PlaceIFood();
  void Update();
};

#endif