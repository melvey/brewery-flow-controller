
void enableFlow();

void flowTick();

boolean updateFlow();

void turnOffAfter(unsigned int targetVolume);

unsigned long calculateFlowRate(long ticks, unsigned long delta);

unsigned long calculateVolume(unsigned long flowRate, unsigned long delta);
