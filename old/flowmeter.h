
void enableFlow();

unsigned long getTicks();

boolean updateFlow();

void turnOffAfter(unsigned long targetVolume);

unsigned long calculateFlowRate(long ticks, unsigned long delta);

unsigned long calculateVolume(unsigned long flowRate, unsigned long delta);
