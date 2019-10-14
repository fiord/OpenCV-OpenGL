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

    Player() {
      this->pos_x = -5.0;
      this->pos_z = 0.0;
      this->pos_y = 1.0;
      this->angle_1 = M_PI / 2.0;
      this->angle_2 = 0;
    }
    Player(double x, double y, double z): pos_x(x), pos_y(y), pos_z(z) {
      this->angle_1 = M_PI / 2.0;
      this->angle_2 = 0.0;
      can_jump = true;
    }
    Player(double angle1, double angle2): angle_1(angle1), angle_2(angle2) {
      this->pos_x = this->pos_z = 0.0;
      this->pos_y = 1.0;
      can_jump = true;
    }
    Player(double pos_x, double pos_y, double pos_z, double angle_1, double angle_2): pos_x(pos_x), pos_y(pos_y), pos_z(pos_z), angle_1(angle_1), angle_2(angle_2) {
      can_jump = true;
    }

    // Playerの視線の先を設定する
    void getTarget(double &x, double &y, double &z) {
      y = this->target_dist * cos(this->angle_1) + this->pos_y;
 x = this->target_dist * sin(this->angle_1) * cos(this->angle_2) + this->pos_x;
 z = this->target_dist * sin(this->angle_1) * sin(this->angle_2) + this->pos_z;
    }
    // Playerの視線移動を行う（マウス操作）
    void moveGaze(int dx, int dy) {
      this->angle_1 -= dy / 300.0;
      this->angle_2 -= dx / 300.0;
      this->angle_1 = std::max(0.0, std::min(M_PI, this->angle_1));
    }
    // Playerの速度変更を行う（AWSD操作と重力）
    void changeVerocity(double dx, double dy, double dz) {
      this->vx += dx;
      if (vx > 1.0)  vx = 1.0;
      else if (vx < -1.0)  vx = -1.0;
      if (this->can_jump) {
        this->vy += dy;
        if (vy > 1.0)  vy = 1.0;
        else if (vy < -1.0)  vy = -1.0;
      }
      if (dy != 0.0) {
        this->can_jump = false;
      }

      this->vz += dz;
      if (this->vz > 1.0)  vz = 1.0;
      else if (this->vz < -1.0)  vz = -1.0;
    }
    // Playerの位置移動を行う
    void movePosition() {
      this->pos_x += this->vx;
      this->pos_y += this->vy;
      this->pos_z += this->vz;
      this->vx *= 0.5;
      this->vz *= 0.5;;
      this->vy += gravity;
      if (this->vy > 1.0)  this->vy = 1.0;
      else if (this->vy < -1.0)  this->vy = -1.0;
      if (this->pos_y <= 0) {
        this->pos_y = 0;
        this->vy = 0;
        this->can_jump = true;
      }
    }
};
#endif
