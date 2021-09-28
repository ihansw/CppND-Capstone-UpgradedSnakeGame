#include "food.h"
#include <iostream>
#include <random>

Food::Food() :
            _x(1),
            _y(1)
            
{}

Food::Food(int food_x, int food_y) :
            _x(food_x),
            _y(food_y)
            
{}      

void Food::PlaceFood(int food_x, int food_y){
  _x = food_x;
  _y = food_y;
}


int Food::getX(){
  return _x;
}

int Food::getY(){
  return _y;
}

    