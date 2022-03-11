#include "timer.hpp"

#include "logger.hpp"

using namespace std;

void Timer::start(void *(*callback)(void *arg), void *arg, uint seconds) {
	// disable timer operations while we start thread
	if (thread == 0 && !lock()) {
		// double check if the timer was not started meanwhile
		if (thread == 0) {
			struct arg_s {
				void *(*callback)(void *arg);
				void * arg;
				uint   seconds;
				Timer *parent;
			};
			auto *ARG = new arg_s({callback, arg, seconds, this});
			pthread_create(
				&thread, nullptr, [](void *arg) -> void * {
					unique_ptr< arg_s > ARG(static_cast< arg_s * >(arg));
					this_thread::sleep_for(chrono::seconds(ARG->seconds));
					pthread_testcancel();
					ARG->parent->thread = 0;
					ARG->callback(ARG->arg);
					return nullptr;
				},
				ARG);
			pthread_detach(thread);
		}
		// enable timer operations again
		unlock();
	}
}

void Timer::stop() {
	// disable timer operations before we attempt to stop
	if (!lock()) {
		// cancel the running, if there is any
		if (thread != 0) {
			pthread_cancel(thread);
			thread = 0;
		}
		// enable timer operations again
		unlock();
	}
}

bool Timer::lockstop() {
	const auto locked = lock();
	stop();
	return locked;
}

#ifdef DEBUG
void Timer::test() {
	Timer t1;	 // t1 should execute and stop t2 before it executes
	Timer t2;	 // t2 should be stopped by t1 or throw an error
	// lets start t1 from 2 threads simultaneously to check for conflicts
	static constexpr auto stopTimer = [](Timer *t) {
		t->stop();
	};
	static constexpr auto startStopper = [](Timer *t1, Timer *t2) {
		t1->start([](void *arg) -> void * {
			Log::Trace("timer triggered as expected");
			// lets simultaneously stop it from 2 threads to test for conflicts
			std::thread thr(stopTimer, static_cast< Timer * >(arg));
			stopTimer(static_cast< Timer * >(arg));
			thr.join();
			return nullptr;
		},
				  t2, 2);
	};
	std::thread thread1(startStopper, &t1, &t2);
	startStopper(&t1, &t2);
	t2.start([](void *arg) -> void * {
		Log::Error("t2 executed!");
		return nullptr;
	},
			 nullptr, 3);
	// as the timer already started, another call to start should not change it
	t1.start([](void *arg) -> void * {
		Log::Error("timer triggered the wrong callback");
		return nullptr;
	},
			 nullptr, 2);
	// lock and try to stop it, which should not work
	t1.lock();
	t1.stop();
	t1.unlock();

	// at last lets start and stop a timer simultaneously (usually a bad race condition ...)
	Timer		t3;
	std::thread thread2(stopTimer, &t3);
	t3.start([](void *arg) -> void * {
		// dont do anything
		return nullptr;
	},
			 nullptr, 0);

	// join threads
	thread1.join();
	thread2.join();

	// wait 4 seconds so all timers expire
	std::this_thread::sleep_for(std::chrono::seconds(4));
}
#endif
