#ifndef EnhancedThread_h
#define EnhancedThread_h

#include "BaseHeader.h"
#include "Thread.h"

class EnhancedThread: public Thread {
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

#endif
