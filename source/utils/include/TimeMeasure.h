/**
 * @file TimeMeasure.h
 * @date Nov 24, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef TIMEMEASURE_H_
#define TIMEMEASURE_H_

///////////////////////////////////////////////////////////////////////////////
// Includes.

#include "log.h"
// Used for PRINT_MEASURED_TIME() macro.
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
// Typedefs.

typedef double Time;

///////////////////////////////////////////////////////////////////////////////
// Enums.

/**
 * When changing this enum also change ClockType enum in TimeMeasure class
 * and switches in {@link Time getTime()}
 * and {@link Time TimeMeasure::getTime()}.
 */
enum ClockType {
	MONOTONIC,
	CLOCK,
	WALL,
	RUSAGE
};

///////////////////////////////////////////////////////////////////////////////
// Functins.


void setDefaultClock(ClockType defaultClock);

ClockType getDefaultClock();

/**
 * Get current time.
 * @return current time.
 */
Time getTime();

Time getTimeMonotonic();

Time getTimeClock();

Time getTimeWall();

Time getTimeRUsage();

///////////////////////////////////////////////////////////////////////////////
// Classes.

class TimeMeasure {
public:


	/**
	 * When changing this enum also change ClockType enum in global space
	 * and switches in {@link Time getTime()}
	 * and {@link Time TimeMeasure::getTime()}.
	 */
	enum ClockType {
		MONOTONIC,
		CLOCK,
		WALL,
		RUSAGE,
		DEFAULT // Always last.
	};

	/**
	 * Construct timer and start timer.
	 * @param clockType clock type.
	 */
	TimeMeasure(ClockType clockType = DEFAULT);

	/**
	 * Start timer.
	 * @return *this.
	 */
	TimeMeasure& start();

	/**
	 * End timer and return elapsed time.
	 * @return elapsed time.
	 */
	Time end() const;

	/**
	 * Get current time.
	 * @return current time.
	 */
	Time getTime() const;

	/**
     * Same as {@link Time end() const}.
     */
	operator Time() const;



protected:
	ClockType _clockType;
	Time _start;
};

///////////////////////////////////////////////////////////////////////////////
// Macros.

#define PRINT_MEASURED_TIME(timer)                    \
	do{                                                \
		verboseLog << __PRETTY_FUNCTION__              \
				<< " " << #timer << " = "              \
				<< std::fixed << (timer).end()         \
				<< std::resetiosflags(std::ios::fixed) \
				<< endl;                               \
	}while(0)

///////////////////////////////////////////////////////////////////////////////

#endif /* TIMEMEASURE_H_ */

