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
      : _grid_width(grid_width),
        _grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update();
  void GrowBody();
  bool SnakeCell(int x, int y);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 protected:
  int _grid_width;
  int _grid_height;

  static bool _USupdated;

  bool _growing{false};

  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
};

// Inherit from Snake to create Opponent snakes
class OppSnake : public Snake {
  public:
    // constructor / desctructor
    OppSnake(int grid_width, int grid_height, int id);
    ~OppSnake();

    bool Update(int random_direction, int US_x, int US_y);
    void UpdateHead(int random_direction);
    int getId();
    
  private:
    int _id;
    int _count_d;
    int _curr_d;    
};

#endif