/**
 * @file TimeMeasure.cpp
 * @date Nov 24, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "TimeMeasure.h"

#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

///////////////////////////////////////////////////////////////////////////////

ClockType _defaultClock = MONOTONIC;

///////////////////////////////////////////////////////////////////////////////

void setDefaultClock(ClockType defaultClock){
	_defaultClock = defaultClock;
}

ClockType getDefaultClock(){
	return _defaultClock;
}

/**
 * Get current time.
 * @return current time.
 */
Time getTime(){
	switch(_defaultClock){
	case MONOTONIC:
		return getTimeMonotonic();
	case CLOCK:
		return getTimeClock();
	case WALL:
		return getTimeWall();
	case RUSAGE:
		return getTimeRUsage();
	}
}

///////////////////////////////////////

Time getTimeMonotonic(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return static_cast<double>(ts.tv_sec)
			+ static_cast<double>(ts.tv_nsec) * 1E-9;
}

Time getTimeClock(){
	return static_cast<double>(clock()) * 1000.0 / CLOCKS_PER_SEC;
}

Time getTimeWall(){
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec)
			+ static_cast<double>(tp.tv_usec) * 1E-6;
}

Time getTimeRUsage(){
	struct rusage ruse;
	getrusage(RUSAGE_SELF, &ruse);
	return static_cast<double>(ruse.ru_utime.tv_sec)
			+ static_cast<double>(ruse.ru_utime.tv_usec) * 1E-6;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Construct timer and start timer.
 * @param clockType clock type.
 */
TimeMeasure::TimeMeasure(ClockType clockType)
		: _clockType(clockType){
	start();
}

/**
 * Start timer.
 * @return *this.
 */
TimeMeasure& TimeMeasure::start(){
	_start = getTime();
	return *this;
}

/**
 * End timer and return elapsed time.
 * @return elapsed time.
 */
Time TimeMeasure::end() const{
	return getTime() - _start;
}

/**
 * Get current time.
 * @return current time.
 */
Time TimeMeasure::getTime() const{
	switch(_clockType){
	case MONOTONIC:
		return ::getTimeMonotonic();
	case CLOCK:
		return ::getTimeClock();
	case WALL:
		return ::getTimeWall();
	case RUSAGE:
		return ::getTimeRUsage();
	case DEFAULT:
		return ::getTime();
	}
}

/**
 * Same as {@link Time end() const}.
 */
TimeMeasure::operator Time() const{
	return end();
}

///////////////////////////////////////////////////////////////////////////////

