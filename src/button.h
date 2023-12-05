// init button + sleep reference pin
void initButton();

void updateButton();

extern struct buttonState_t {
  bool pressed;
  bool released;
} buttonState;
