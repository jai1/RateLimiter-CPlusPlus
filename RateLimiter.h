#ifndef RATELIMITER_H_
#define RATELIMITER_H_

#include <chrono>
#include <thread>
#include <boost/thread/mutex.hpp>

typedef std::chrono::high_resolution_clock Clock;
typedef boost::unique_lock<boost::mutex> Lock;

class RateLimiter {
public:
    RateLimiter(double rateInPermitsPerSecond, unsigned long maxServablePermits);
    // Using overloading instead of default arguments,
    // since another argument cannot be used as the default value
    // because the evaluation of function arguments is not sequenced.
    RateLimiter(double rateInPermitsPerSecond);
    void aquire(unsigned long requestedPermits=1);
    bool tryAquire(unsigned long requestedPermits=1, Clock::duration timeout = std::chrono::seconds(0));
private:
    boost::mutex mutex_;
    const unsigned long maxServablePermits_;
    unsigned long unusedPermits_;
    Clock::duration intervalInMillisecondsBetweenPermits_;
    Clock::time_point lastPermitAcquiredTime_;
    void aquireWithinLock(unsigned long requestedPermits=1);
};

#endif /* RATELIMITER_H_ */
