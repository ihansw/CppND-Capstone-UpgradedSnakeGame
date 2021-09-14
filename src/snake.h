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

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 //private:
  

 protected:
  int grid_width;
  int grid_height;

  static bool _USupdated;

  bool growing{false};

  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
};

// Inheritance
class OppSnake : public Snake {
  public:
    // constructor / desctructor
    OppSnake(int grid_width, int grid_height, int id);
    ~OppSnake();

    void simulate();
    void Update(int random_direction);
    void UpdateHead(int random_direction);
    int getId();

    std::mutex _mtx;
    
  private:
    int _id;
    int _count_d;
    int _curr_d;

    // TODO: Need to revisit. This is saying that each instance has _threads??
    std::vector<std::thread> _threads;
    void move();
    
};

#endif