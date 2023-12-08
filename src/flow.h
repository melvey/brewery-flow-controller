#ifndef FLOW_H
#define FLOW_H

#define OVERFLOW_TICKS 65535
#define TICKS_PER_LITRE 356


unsigned long calculateFlowRate(int ticks);
unsigned long calculateVolume(int ticks, int volOverflows);
void updateVolume(int newTicks);
void openFlow(void);
void closeFlow(void);
void setTargetVolume(unsigned int newVolume);
unsigned int getTargetVolume(void);

unsigned long getFlowRate(void);
unsigned long getVolume(void);
unsigned long getFlowStatus(void);

#endif
