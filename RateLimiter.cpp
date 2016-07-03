#include "RateLimiter.h"
#include <boost/thread/locks.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <algorithm>
#include <cassert>

RateLimiter::RateLimiter(double rateInPermitsPerSecond)
    :  RateLimiter(rateInPermitsPerSecond, (unsigned long) rateInPermitsPerSecond){
}

RateLimiter::RateLimiter(double rateInPermitsPerSecond, unsigned long maxServablePermits)
    : maxServablePermits_(maxServablePermits) {
    // 1e6 = 10^6
    assert(rateInPermitsPerSecond < 1e6 &&
           "Can't have a rate less than 1 permit per microsecond (10^-6 permits/seconds).");
    intervalInMillisecondsBetweenPermits_ = std::chrono::microseconds((long) (1e6 / rateInPermitsPerSecond));
    lastPermitAcquiredTime_ = Clock::now();
    unusedPermits_ = 0;
}

void RateLimiter::aquire(unsigned long requestedPermits) {
	Lock lock(mutex_);
	aquireWithinLock(requestedPermits);
}

bool RateLimiter::tryAquire(unsigned long requestedPermits, Clock::duration timeout) {
	Lock lock(mutex_);
	Clock::duration timeElapsed = Clock::now() + timeout - lastPermitAcquiredTime_;
	unsigned long availablePermits = std::min<unsigned long>(maxServablePermits_, unusedPermits_ + timeElapsed / intervalInMillisecondsBetweenPermits_);
	if (requestedPermits > availablePermits) {
		return false;
	}
	aquireWithinLock(requestedPermits);
	return true;
}

void RateLimiter::aquireWithinLock(unsigned long requestedPermits) {
    Clock::time_point now = Clock::now();
    assert(now >= lastPermitAcquiredTime_ && "Current Time can't be less than last permit acquired");

    Clock::duration timeElapsed = now - lastPermitAcquiredTime_;
    unusedPermits_ = std::min<unsigned long>(maxServablePermits_, unusedPermits_ + timeElapsed / intervalInMillisecondsBetweenPermits_);
    unsigned long neededPermits = requestedPermits - std::min<unsigned long>(unusedPermits_, requestedPermits);
    // std::min guarantees that neededPermits>=0
    assert(neededPermits>=0 && "Needed permits can't be negative");

    if (neededPermits != 0) {
        std::this_thread::sleep_for(neededPermits * intervalInMillisecondsBetweenPermits_);
    }
    lastPermitAcquiredTime_ = Clock::now();
}
