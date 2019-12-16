typedef struct Street;
void initializeStreet (void);
void enterStreet (enum Direction d);
void leaveStreet (void);
void recordWaitingTime (int waitingTime);
void* cars ();