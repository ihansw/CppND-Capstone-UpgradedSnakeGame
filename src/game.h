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
  
 private:
  Snake _snake;
  std::vector<std::shared_ptr<OppSnake>> _opp_snakes;
  int _n_opp_snakes;
  std::vector<Food> _foods;
  std::shared_ptr<Food> _i_food;
  std::shared_ptr<ImmortalMode> _i_mode;

  int _grid_width;
  int _grid_height;

  int _score{0};

  std::random_device _dev;
  std::mt19937 _engine;
  std::uniform_int_distribution<int> _random_w;
  std::uniform_int_distribution<int> _random_h;
  std::uniform_int_distribution<int> _random_d;

  void PlaceIFood();
  void Update();
};

#endif