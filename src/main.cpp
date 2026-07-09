#include<chrono>
#include<iostream>

#include "RateLimiter.h"

int main(){

    RateLimiter limiter(5,std::chrono::seconds(60));

    std::cout << std::boolalpha;

    std::cout << limiter.allowRequests("Alice") << '\n';

    return 0;
}