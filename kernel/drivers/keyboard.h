#ifndef ARCH_AMD64_DRIVERS_KEYBOARD_H
#define ARCH_AMD64_DRIVERS_KEYBOARD_H

void keyboard_irq(void);
bool get_key(char *c);
void keyboard_init(void);
void keyboard_wait(void);

#endif