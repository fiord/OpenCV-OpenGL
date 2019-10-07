#ifndef PLAYER_INCLUDED
#include <cmath>
#include <algorithm>
#define PLAYER_INCLUDED

class Player {
  public:
    double pos_x, pos_y, pos_z;
    double vx, vy, vz;
    double angle_1, angle_2;
    bool can_jump;
    const double target_dist = 10.0;
    const double gravity = -0.05;

    Player();
    Player(double, double, double);
    Player(double, double);
    Player(double, double, double, double, double);

    void getTarget(double &x, double &y, double &z);
    void moveGaze(int, int);
    void changeVerocity(double, double, double);
    void movePosition();
};
#endif
