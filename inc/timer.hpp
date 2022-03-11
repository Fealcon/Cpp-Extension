#pragma once

#include <thread>
#include <atomic>

/// The Timer class provides a thread safe timer.
class Timer {
	std::atomic_flag flag	= ATOMIC_FLAG_INIT;	   ///< This flag disables #start and #stop of the Timer.
	pthread_t		 thread = 0;	///< A thread, where the timer can wait for execution.

  public:
	~Timer() {
		pthread_cancel(thread);
	}

	/// Start the timer and execute \a callback on \a arg after \a seconds, if the timer was not stopped.
	void start(void *(*callback)(void *arg), void *arg, uint seconds);
	/// Stop the currently running timer, if any, and prevent the callback from execution.
	void stop();
	/*! \brief Lock (i.e. disable) #start and #stop. It won't stop the Timer.
	 *  \return \c TRUE, if the Timer was locked before.
	 */
	bool lock() { return flag.test_and_set(); }
	/// #lock and #stop.
	bool lockstop();
	/// Unlock (i.e. enable) #start and #stop.
	void unlock() { flag.clear(); }

#ifdef DEBUG
	/// test will test the Timer implementation for correct behavior
	static void test();
#endif
};
