#include <iostream>
#include <random>
#include <stdexcept>

#include "boid.hpp"
#include "flock.hpp"

void ignoreLine() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
} 

int main() {
  try {
    bd::Flock flock1;
    int N{};
    bd::Parameters par1{};
    char cmd;

    const double duration = 10.0;
    const double delta_t = 1.0;
    double time{};

    std::vector<double> av_distances{};
    std::vector<double> s_distances{};

    std::vector<double> av_speeds{};
    std::vector<double> s_speeds{};

    double norm{};
    double norm2{};

    std::cout
        << "Valid commands: \n"
        << "- generate a flock [g]\n"
        << "- compute statistics and print to screen [s]\n"
        << "- print histograms to screen [h NORM(distance) NORM(speeds)]\n"
        << "- quit [q]\n";

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    double screenWidth = desktop.width;
    double screenHeight = desktop.height;

    std::random_device r;
    std::default_random_engine eng(r());
    std::uniform_real_distribution<double> xDist(0, screenWidth);
    std::uniform_real_distribution<double> yDist(0, screenHeight);
    std::uniform_real_distribution<double> vxDist(-1, 1);
    std::uniform_real_distribution<double> vyDist(-1, 1);

    while (std::cin >> cmd) {
      if (cmd == 'g') {
        // to avoid including the previous data when pressing 'g' twice:
        av_distances.clear();
        s_distances.clear();
        av_speeds.clear();
        s_speeds.clear();

        time = 0.;

        std::cout << "Input the parameters: d, ds, s, a, c \n";

        std::cin >> par1.d >> par1.ds >> par1.s >> par1.a >> par1.c;
        ignoreLine();

        std::cout << "Input the number of boids: ";

        std::cin >> N;
        ignoreLine();

        if (N < 2) {
          throw std::runtime_error{
              "Not enough data. Try generating a bigger flock.\n"};
        }

        for (int i = 0; i < N; i++) {
          double posX = xDist(eng);
          double posY = yDist(eng);
          double velX = vxDist(eng);
          double velY = vyDist(eng);
          sf::Vector2<double> vel{velX, velY};

          bd::Boid newBoid(posX, posY);
          newBoid.setMaxspeed(500);
          newBoid.setVelocity(vel);

          flock1.addBoid(newBoid);
        }
        flock1.setParameters(par1);

        while (time < duration) {
          double a_d = flock1.average_distance().mean;
          double s_d = flock1.average_distance().sigma;

          av_distances.push_back(a_d);
          s_distances.push_back(s_d);

          double a_s = flock1.average_speed().mean;
          double s_s = flock1.average_speed().sigma;

          av_speeds.push_back(a_s); //average speeds
          s_speeds.push_back(s_s); //uncertainties 

          flock1.updateFlock(delta_t);

          time += delta_t;
        }
        std::cout << "Data generated successfully\n";
      } else if (cmd == 's') {
        if (N == 0) {
          throw std::runtime_error{
              "Not enough data. Try generating a flock with [g].\n"};
        }
        //printing statistics to screen:
        for (int i = 0, N = av_distances.size(); i < N; i++) {
          std::cout << "Average distance = " << av_distances[i] << " +- "
                    << s_distances[i] << "\n";
          std::cout << "Average speed = " << av_speeds[i] << " +- "
                    << s_speeds[i] << "\n\n";
        }

      } else if (cmd == 'h' && std::cin >> norm >> norm2) {
        if (norm < 1 || norm2 < 1) {
          throw std::runtime_error{
              "Norms need to be positive non-null numbers to visualize the "
              "histogram correctly.\n"};
        }
        //printing histogram:
        std::cout << "Histogram of mean distances:\n"
                  << "\n";
        bd::histogram(av_distances, s_distances, norm);
        std::cout << "\nHistogram of mean speeds:\n"
                  << "\n";
        bd::histogram(av_speeds, s_speeds, norm2);

      } else if (cmd == 'q') { //exit program
        return EXIT_SUCCESS;
      } else {
        std::cout << "Bad format, insert a new command\n";
        std::cin.clear();
        ignoreLine();
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "Caught exception: '" << e.what() << "'\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Caught unknown exception\n";
    return EXIT_FAILURE;
  }
}