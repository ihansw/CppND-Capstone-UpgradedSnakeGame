#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <thread>
#include <mutex>
#include "SDL.h"

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);
  int get_grid_width();
  int get_grid_height();

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 private:
  virtual void UpdateHead();
  virtual void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  bool growing{false};
  int grid_width;
  int grid_height;

 protected:
  static bool _USupdated;
};

// Inheritance
class OppSnake : public Snake {
  public:
    // constructor / desctructor
    OppSnake(int grid_width, int grid_height, int id);
    ~OppSnake();

    void simulate();
    int updatePos();
    int getId();
    std::mutex _mtx;
    
  private:
    int _id;
    std::vector<std::thread> _threads;
    void move();

    //void UpdateHead() override;
    //void UpdateBody() override;
    
};

#endif