#pragma once
#ifndef FLOCK_HPP
#define FLOCK_HPP

#include "boid.hpp"

namespace bd {

  void histogram(std::vector<double>, std::vector<double>, double norm);

  struct Statistics{
    double mean{};
    double sigma{};
  };

  struct Color{
    double red{};
    double green{};
    double blue{};
  };

class Flock {
  std::vector<Boid> m_flock;
  Color f_color;
  

 public:

  int size() const { return m_flock.size(); }

  auto& flock() { return m_flock; }
  const auto& flock() const { return m_flock; }


  Boid getBoid(int i) const;
  Boid& getBoid(int i);

  void addBoid(const Boid& b);

  void updateFlock(double const delta_t);

  Statistics average_distance();

  Statistics average_speed();

  void setParameters(const Parameters& par1);

  void setColor(const Color& c1);
  Color getColor();

  void resetFlock();

};

}  // namespace bd

#endif