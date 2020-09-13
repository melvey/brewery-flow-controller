
bool isOpen();

unsigned long getTotalVolume();

unsigned long getRate();

unsigned long getLimit();

unsigned long getCurrentVolume();

unsigned long getLastSampleTime();

bool pastLimit();

void setOpen(bool valveOpen);

void setRate(unsigned long rate);

void addVolume(unsigned long volume);

void resetVolume();

void resetTotalVolume();

void setLimit(unsigned int limit);

void setLastSampleTime(unsigned long sampleTime);
