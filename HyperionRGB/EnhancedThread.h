#ifndef EnhancedThread_h
#define EnhancedThread_h

#include "BaseHeader.h"
#include <Thread.h>

class EnhancedThread : public Thread {
  public:
    void
      run(void),
      runIfNeeded(void),
      reset(void),
      setRunOnce(bool);
    unsigned long getInterval(void);
  private:
    bool _runOnce;
};

void EnhancedThread::run() {
  Thread::run();
  if (_runOnce)
    Thread::enabled = false;
}

void EnhancedThread::runIfNeeded(void) {
  if(Thread::shouldRun())
    Thread::run();
}

void EnhancedThread::reset(void) {
  Thread::enabled = true;
  Thread::runned();
}

void EnhancedThread::setRunOnce(bool runOnce) {
  _runOnce = runOnce;
}
unsigned long EnhancedThread::getInterval(void) {
  return interval;
}

#endif
