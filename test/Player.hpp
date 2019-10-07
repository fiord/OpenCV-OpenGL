#ifndef PLAYER_INCLUDED
#include <cmath>
#include <algorithm>
#define PLAYER_INCLUDED

class Player {
  public:
    double pos_x, pos_y, pos_z;
    double vx, vy, vz;
    double angle_1, angle_2;
    const double target_dist = 10.0;

    Player();
    Player(double, double, double);
    Player(double, double);
    Player(double, double, double, double, double);

    void getTarget(double &x, double &y, double &z);
    void moveGaze(int, int);
    void changeVerocity(int, int, int);
    void movePosition();
};
#endif
