#ifndef FOOD_H
#define FOOD_H

class Food {
  public:
    Food();
    Food(int food_x, int food_y);
    //~Food();

    void PlaceFood(int food_x, int food_y);
    int getX();
    int getY();

  private:
    int _x;
    int _y;

};

#endif