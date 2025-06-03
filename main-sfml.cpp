#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "boid.hpp"
#include "flock.hpp"

void ignoreLine() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
  try {
    int N{};  // number of boids
    char cmd;

    bd::Flock flock1;
    std::vector<bd::Flock> flocks;
    const int screenWidth{1280};
    const int screenHeight{720};
    const float triangleSide{4};  // length of side

    std::random_device r;
    std::default_random_engine eng(r());
    std::uniform_real_distribution<double> xDist(0, screenWidth);
    std::uniform_real_distribution<double> yDist(0, screenHeight);
    std::uniform_real_distribution<double> vxDist(-1, 1);
    std::uniform_real_distribution<double> vyDist(-1, 1);
    std::uniform_real_distribution<double> R(0, 255);
    std::uniform_real_distribution<double> G(0, 255);
    std::uniform_real_distribution<double> B(0, 255);

    bd::Parameters params;
    bd::Color r_color;


    std::cout << "Valid commands:\n"
              << "[g] to generate a flock\n"
              << "[f] to view the boids\n"
              << "[q] to quit.\n";

    while (std::cin >> cmd) {
      switch (cmd) {
        case 'g': {
          // to avoid including the previous data when pressing 'g' twice:
          flock1.resetFlock();

          std::cout
              << "Please input your data. \nFirst enter the number of boids: ";
          std::cin >> N;
          ignoreLine();

          if (N < 2) {
            throw std::runtime_error{
                "Not enough data. Try generating a bigger flock.\n"};
          }

          std::cout << "Enter separation parameter s: ";
          std::cin >> params.s;
          ignoreLine();

          std::cout << "Enter alignment parameter a: ";
          std::cin >> params.a;
          ignoreLine();

          std::cout << "Enter cohesion parameter c: ";
          std::cin >> params.c;
          ignoreLine();

          std::cout << "Enter distance d and range influence parameter ds: ";
          std::cin >> params.d >> params.ds;

          if (std::cin.fail()) {
            std::cin.clear();
            ignoreLine();
          }

          // Initialize boids:
          for (int i = 0; i < N; ++i) {
            double x = xDist(eng);
            double y = yDist(eng);
            double vx = vxDist(eng);
            double vy = vyDist(eng);
            bd::Boid boid(x, y);
            boid.setVelocity({vx, vy});
            boid.setMaxspeed(400);

            boid.setPar(params);

            flock1.addBoid(boid);
          }

          r_color.red = R(eng);
          r_color.green = G(eng);
          r_color.blue = B(eng);

          flock1.setColor(r_color);

          flocks.push_back(flock1);
          std::cout << "Data generated successfully.\n";

          break;
        }

        case 'f': {
          if (N == 0) {
            throw std::runtime_error{
                "Not enough data. Try generating a flock with [g].\n"};
          }

          sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight),
                                  "Boids");
          window.setFramerateLimit(60);

          sf::Clock clock;

          while (window.isOpen()) {
            sf::Event event;
            sf::Time elapsed = clock.restart();
            // boid movement independent from frame rate!
            double delta_t = elapsed.asSeconds();

            while (window.pollEvent(event)) {
              if ((event.type ==
                   sf::Event::Closed) ||  // press X button to quit
                  (sf::Keyboard::isKeyPressed(
                      sf::Keyboard::Escape)))  // press esc to quit
              {
                window.close();
              }
            }

            window.clear();
            for (bd::Flock& flock1 : flocks){
              flock1.updateFlock(delta_t);

              // Draw boids on screen:
              for (const bd::Boid& boid : flock1.flock()) {
                sf::ConvexShape shape;
                double rotation{};
                shape.setPosition(boid.getPosition().x, boid.getPosition().y);
                shape.setPointCount(3);  // creating isosceles triangles:
                shape.setPoint(0, sf::Vector2f(-triangleSide, triangleSide));
                shape.setPoint(1, sf::Vector2f(triangleSide, triangleSide));
                shape.setPoint(2, sf::Vector2f(0, 5 * triangleSide));

                sf::Color randomColor(flock1.getColor().red, flock1.getColor().green, flock1.getColor().blue);

                shape.setFillColor(randomColor);
                rotation = bd::angle(boid.getVelocity());
                // converting from radians to degrees
                // with a +270 degrees correction needed because of the window's
                // origin position
                shape.setRotation(270 + (rotation * 180) / M_PI);

                window.draw(shape);
              
            }}

            window.display();
          }
          break;
        }
        case 'q': {  // exit program
          return EXIT_SUCCESS;
          break;
        }
        default: {
          std::cout << "Bad format, insert a new command\n";
          std::cin.clear();
          ignoreLine();
        }
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "An exception occurred: " << e.what() << "'\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Caught unknown exception\n";
    return EXIT_FAILURE;
  }
}