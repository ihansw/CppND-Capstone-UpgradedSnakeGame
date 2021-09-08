#include "snake.h"
#include <cmath>
#include <iostream>

//Snake::Snake(int grid_width, int grid_height)
void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

// Constructor
OppSnake::OppSnake(int grid_width, int grid_height, int id): Snake(grid_width, grid_height) {
  // set id
  id = id;

  // head_x & head_y are already initialized using on the base class.
  // update head_x & head_y based on the id number
  switch(id){
    case 0:
      head_x = grid_width / 4;
      head_y = grid_height / 4;
      break;
    case 1:
      head_x = grid_width / 2;
      head_y = grid_height / 4;
      break;
    case 2:
      head_x = grid_width * 3 / 4;
      head_y = grid_height / 4;
      break;
    case 3:
      head_x = grid_width / 4;
      head_y = grid_height / 2;
      break;
    case 4:
      head_x = grid_width * 3 / 4;
      head_y = grid_height / 2;
      break;
    case 5:
      head_x = grid_width / 4;
      head_y = grid_height * 3/ 4;
      break;
    case 6:
      head_x = grid_width / 2;
      head_y = grid_height * 3/ 4;
      break;
    case 7:
      head_x = grid_width * 3/ 4;
      head_y = grid_height * 3/ 4;
      break;
    default:
        std::cout << "Invalid Number of Opponent Snakes" << std::endl;
  }

}

// Desctructor
OppSnake::~OppSnake(){

}