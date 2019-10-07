#include "Player.hpp"

Player::Player() {
 this->pos_x = -5.0;
 this->pos_z = 0.0;
 this->pos_y = 1.0;
 this->angle_1 = M_PI / 2.0;
 this->angle_2 = 0;
}

Player::Player(double x, double y, double z): pos_x(x), pos_y(y), pos_z(z) {
 this->angle_1 = M_PI / 2.0;
 this->angle_2 = 0.0;
}
Player::Player(double angle1, double angle2): angle_1(angle1), angle_2(angle2) {
 this->pos_x = this->pos_z = 0.0;
 this->pos_y = 1.0;
}
Player::Player(double x, double y, double z, double angle1, double angle2): pos_x(x), pos_y(y), pos_z(z), angle_1(angle1), angle_2(angle2) {}

// Playerの視線の先を設定する
void Player::getTarget(double &x, double &y, double &z) {
 y = this->target_dist * cos(this->angle_1) + this->pos_y;
 x = this->target_dist * sin(this->angle_1) * cos(this->angle_2) + this->pos_x;
 z = this->target_dist * sin(this->angle_1) * sin(this->angle_2) + this->pos_z;
}

// Playerの視線移動を行う（マウス操作）
void Player::moveGaze(int dx, int dy) {
 this->angle_1 -= dy / 300.0;
 this->angle_2 -= dx / 300.0;
 this->angle_1 = std::max(0.0, std::min(M_PI, this->angle_1));
}

// Playerの速度変更を行う（AWSD操作と重力）
void Player::changeVerocity(int dx, int dy, int dz) {
 this->vx += dx;
 this->vy += dy;
 this->vz += dz;
}

// Playerの位置移動を行う
void Player::movePosition() {
 this->pos_x += this->vx;
 this->pos_y += this->vy;
 this->pos_z += this->vz;
 this->vx *= 0.5;
 this->vy *= 0.5;
 this->vz *= 0.5;
}
