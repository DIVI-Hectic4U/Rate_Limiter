#include "../../include/simulator/Simulator.h"

#include <iostream>

Simulator::Simulator(FixedWindowLimiter& limiter)
    : limiter_(limiter)
{

}

void Simulator::run()
{
    std::cout << std::boolalpha;

    std::cout << "\n== Alice ==\n";

    for(int i = 1 ; i <= 7 ; i++){
        std::cout
            << "Request "
            << i
            << " : "
            << limiter_.allowRequest("Alice")
            << '\n';
    }

    std:: cout << "\n== Bob ==\n";

    for(int i = 1 ; i <= 3; i++){
        std:: cout
            << "Request "
            << i
            << " : "
            <<  limiter_.allowRequest("Bob")
            << '\n';
    }
}