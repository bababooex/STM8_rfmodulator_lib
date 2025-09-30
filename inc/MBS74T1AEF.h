#ifndef RFMOD_H
#define RFMOD_H

#include "swi2c.h"
#include "stdbool.h"
// MBS74T1AEF default address
#define RFMOD_ADDR   (0x65 << 1)

// audio sound system functions
typedef enum {
    MN_4_5MHZ = 0,
    BG_5_5MHZ,
    I_6_0MHZ,
    DK_6_5MHZ
} rfmod_sound_system;

/**
 * @brief functions regarding rf modulator
 * - start rf modulator
 * - put rf modulator to sleep
 * - change channel anytime
 * - change sound frequency anytime
 * - experimental frequency mode
 */
void rfmod_start(uint8_t channel, rfmod_sound_system sound, bool testMode);
void rfmod_sleep(void);
void rfmod_changeChannel(uint8_t channel, bool testMode);
void rfmod_setSoundCarrier(rfmod_sound_system sound);
void rfmod_setFrequencyRaw(float freqMHz, rfmod_sound_system sound, bool testMode);
uint8_t rfmod_getStatus(void);
#endif // RFMOD_H