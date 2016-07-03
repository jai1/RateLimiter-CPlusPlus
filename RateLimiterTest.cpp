#include "RateLimiter.h"
#include <iostream>
#include <ctime>
#include <stdlib.h>

inline 
std::chrono::time_point <std::chrono::high_resolution_clock> 
now() 
{ 
  return std::chrono::high_resolution_clock::now(); 
}

bool testAcquireBeforeInterval(double rate, long numberOfIterations, int minPermits, int maxPermits);

int main() {
    testAcquireBeforeInterval(0.5, 5, 3, 10);
    testAcquireBeforeInterval(2, 10, 0, 30);
    return 0;
}


bool testAcquireBeforeInterval(double rate, long numberOfIterations, int minPermits, int maxPermits) {
    RateLimiter r(rate);
    srand (time(NULL));
    std::cout<<"++++++++++++++++++++ rate = "<<rate<<" +++++++++++++++++++++++++++++++"<<std::endl;

    while (numberOfIterations--) {
        auto start_time = now();
        int numberOfPermits = rand() % (maxPermits - minPermits) + minPermits;
        r.aquire(numberOfPermits);
        auto stop_time = now();
        std::cout<<"<<<<<<<<<<<<< START <<<<<<<<<<<<<<<"<<std::endl;
        double expectedInSeconds = numberOfPermits/rate;
        double actualInSeconds = (std::chrono::duration_cast<std::chrono::seconds>( stop_time - start_time )).count();
        std::cout<<"Expected to acquire permits in "<<numberOfPermits/rate<<" seconds"<<std::endl;
        std::cout<<"Acquired permits in "<<  (std::chrono::duration_cast<std::chrono::milliseconds>( stop_time - start_time )).count() <<" milli seconds."<<std::endl;
        std::cout<<"Acquired permits in "<<  (std::chrono::duration_cast<std::chrono::seconds>( stop_time - start_time )).count() <<" seconds."<<std::endl;
        std::cout<<"numberOfPermits = "<<numberOfPermits<<std::endl;
        double absDifferenceInSeconds = abs(actualInSeconds - expectedInSeconds);
        assert(absDifferenceInSeconds < 1 && absDifferenceInSeconds >= 0 && "ASSERTION ERROR");
        std::cout<<">>>>>>>>>>>>> END >>>>>>>>>>>>>>>>>\n"<<std::endl;
    }
    return true;
}
