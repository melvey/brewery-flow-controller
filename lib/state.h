
bool isOpen();

unsigned long getTotalVolume();

unsigned long getTotalVolumeInMl();

unsigned long getRate();

unsigned long getLimit();

unsigned long getCurrentVolume();

unsigned long getCurrentVolumeInMl();

unsigned long getLastSampleTime();

bool pastLimit();

void setOpen(bool valveOpen);

void setRate(unsigned long rate);

void addVolume(unsigned long volume);

void resetVolume();

void resetTotalVolume();

void setLimit(unsigned long limit);

void setLastSampleTime(unsigned long sampleTime);

void addTick();
