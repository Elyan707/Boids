#include "boid.hpp"

#include <cassert>
#include <cmath>
#include <iostream>


namespace bd {
// distance between two vectors:

double e_distance(const sf::Vector2<double>& vec1,
                const sf::Vector2<double>& vec2) {
  double dx = vec2.x - vec1.x;
  double dy = vec2.y - vec1.y;

  return std::sqrt(dx * dx + dy * dy);
}

double distance(const sf::Vector2<double>& vec1,
                const sf::Vector2<double>& vec2) {
  double dx = vec2.x - vec1.x;
  double dy = vec2.y - vec1.y;

  if(dx > 1280/2) dx = 1280 -dx;
  if(dy > 720/2) dy = 720 - dy;

  return std::sqrt(dx * dx + dy * dy);
}
// absolute value of a vector:
double magnitude(const sf::Vector2<double>& vec) {
  return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}
// rotation angle of a boid, given its velocity:
double angle(const sf::Vector2<double>& v) { return std::atan2(v.y, v.x); }

Boid::Boid() : position(0, 0) {}
Boid::Boid(double pos_x, double pos_y) : position(pos_x, pos_y) {}

sf::Vector2<double> Boid::getPosition() const { return position; }
void Boid::setPosition(const sf::Vector2<double>& newPos) { position = newPos; }

sf::Vector2<double> Boid::getVelocity() const { return velocity; }
void Boid::setVelocity(const sf::Vector2<double>& newVel) { velocity = newVel; }

Parameters Boid::getPar() const { return par; }
void Boid::setPar(const Parameters& newPar) {
  par = newPar;

  if (par.d < 0.) {
    throw std::runtime_error{
        "Something went wrong. Parameter d must be positive.\n"};
  }

  if (par.ds < 0. || par.ds >= par.d) {
    throw std::runtime_error{
        "Something went wrong. Parameter ds must be positive and "
        "smaller than d.\n"};
  }

  if (par.s < 0. || par.s > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter s must be a number between 0 "
        "and 1.\n"};
  }

  if (par.a < 0. || par.a > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter a must be a number between 0 "
        "and 1.\n"};
  }

  if (par.c < 0. || par.c > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter c must be a number between 0 "
        "and 1.\n"};
  }
}

void Boid::setPar_d(const double new_d) {
  par.d = new_d;

  if (par.d < 0.) {
    throw std::runtime_error{
        "Something went wrong. Parameter d must be positive.\n"};
  }
}

void Boid::setPar_ds(const double new_ds) {
  par.ds = new_ds;

  if (par.ds < 0. || par.ds >= par.d) {
    throw std::runtime_error{
        "Something went wrong. Parameter ds must be positive and "
        "smaller than d.\n"};
  }
}

void Boid::setPar_s(const double new_s) {
  par.s = new_s;

  if (par.s < 0. || par.s > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter s must be a number between 0 "
        "and 1.\n"};
  }
}

void Boid::setPar_a(const double new_a) {
  par.a = new_a;

  if (par.a < 0. || par.a > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter a must be a number between 0 "
        "and 1.\n"};
  }
}

void Boid::setPar_c(const double new_c) {
  par.c = new_c;

  if (par.c < 0. || par.c > 1.) {
    throw std::runtime_error{
        "Something went wrong. Parameter c must be a number between 0 "
        "and 1.\n"};
  }
}

double Boid::getMaxspeed() const { return maxspeed; }
void Boid::setMaxspeed(double new_Maxspeed) { maxspeed = new_Maxspeed; }

sf::Vector2<double> Boid::separation(const std::vector<Boid>& boids) {
  double ds = par.ds;
  double s = par.s;

  sf::Vector2<double> Displacements(0, 0);

  for (auto const& boid : boids) {
    const sf::Vector2<double>& otherPosition = boid.position;
    double distance1 = distance(position, otherPosition);
    assert(distance1 >= 0.0);

    if (distance1 < ds) {
      sf::Vector2<double> displacement = otherPosition - position;
      Displacements += displacement;
    }
  }
  sf::Vector2<double> v1 = -s * Displacements;
  return v1;
}

sf::Vector2<double> Boid::alignment(const std::vector<Boid>& boids) {
  double a = par.a;
  double d = par.d;
  int N{};

  sf::Vector2<double> v2(0, 0);
  sf::Vector2<double> Velocities(0, 0);

  for (auto const& boid : boids) {
    double distance1 = distance(position, boid.position);
    assert(distance1 >= 0.0);
    if (distance1 < d) {
      sf::Vector2<double> speed = boid.velocity - velocity;
      Velocities += speed;
      N++;
    }
  }
  if (N > 1) {
    v2 = a * (1.0 / (N - 1)) * Velocities;
  }
  return v2;
}

sf::Vector2<double> Boid::cohesion(const std::vector<Boid>& boids) {
  double c = par.c;
  double d = par.d;
  int N{};

  sf::Vector2<double> sum_pos(0, 0);
  sf::Vector2<double> v3(0, 0);

  for (auto const& boid : boids) {
    double distance1 = e_distance(position, boid.position);
    //double distance2 = distance(position, boid.position);

    assert(distance1 >= 0.0);
    if (distance1 < d) {
      sf::Vector2<double> otherPosition = boid.position;
      sum_pos += otherPosition;
      N++;
    }/*else if(distance2 < d && distance1>d){
      sf::Vector2<double> otherPosition = boid.position;
      position.x += 1780 - (position.x - otherPosition.x);
      position.y +=  720 - (position.y - otherPosition.y);

      sum_pos += position;
      N++;
    }*/
  }
  sum_pos -= position;
  if (N > 1) {
    sf::Vector2<double> xc = (1.0 / (N - 1)) * sum_pos;
    v3 = c * (xc - position);
  }
  return v3;
}

void Boid::updateVelocity(const std::vector<Boid>& boids) {
  sf::Vector2<double> v1 = separation(boids);
  sf::Vector2<double> v2 = alignment(boids);
  sf::Vector2<double> v3 = cohesion(boids);

  velocity += v1 + v2 + v3;

  double mag_v = magnitude(velocity);
  assert(mag_v >= 0.0);

  // if the absolute value of velocity is greater than the max speed, it gets
  // rescaled within range:
  if (mag_v > maxspeed) {
    velocity.x = (velocity.x / mag_v) * maxspeed;
    velocity.y = (velocity.y / mag_v) * maxspeed;
  };
}
void Boid::updatePosition(double const delta_t) {
  position += velocity * delta_t;
}

// checking borders behavior.
// toroidal space: if a point exits from the screen
// it gets transported back in on the opposite side
void Boid::borders() {
  double screenWidth{1280};
  double screenHeight{720};

  if (position.x < 0.) {
    position.x = screenWidth;
  } else if (position.x > screenWidth) {
    position.x = 0;
  }
  if (position.y < 0.) {
    position.y = screenHeight;
  } else if (position.y > screenHeight) {
    position.y = 0;
  }
}

void Boid::update(const std::vector<Boid>& boids, double const delta_t) {
  updateVelocity(boids);
  updatePosition(delta_t);
  borders();
}

}  // namespace bd