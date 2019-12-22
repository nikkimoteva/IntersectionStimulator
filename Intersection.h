struct Street;
enum Direction {NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3};
const static enum Direction oppositeEnd [] = {SOUTH, NORTH, WEST, EAST};
void initializeStreet (void);
void destroyStreet (void);
void enterStreet (enum Direction d);
void leaveStreet (void);
void waitingTime (int waitingTime);
void* cars ();