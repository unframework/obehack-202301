// init button + sleep reference pin
void initButton();

void updateButton();

// reflects the momentary change from the last frame
// (i.e. pressed/released are true only until the next call to updateButton())
struct buttonState_t {
  bool pressed;
  bool released;
};

extern struct buttonState_t buttonState;
extern struct buttonState_t longPressState;
