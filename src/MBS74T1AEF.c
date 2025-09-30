#include "mbs74t1aef.h"
//main functions
uint16_t freq;
uint8_t fm, fl;
uint8_t data[4];
uint8_t status;
uint16_t N;
uint8_t divider, X;
//
static const uint8_t rfmod_sound[4] = { 0x00, 0x08, 0x10, 0x18 };

// fm / fl byte helper, ensure valid channel range, also TPEN enable
static void rfmod_setFreqBytes(uint8_t *fm, uint8_t *fl, uint8_t channel, bool testMode) {
    if (channel < 21 || channel > 69) {
        channel = 21; 
    }
    //function to change frequency to coresponding channel
    freq = 32 * channel + 1213;
    *fm = (freq >> 6) & 0xFF;
    if (testMode) {
        *fm |= 0x40; 
    }
    *fl = (freq & 0x3F) << 2;
}

// Start the modulator on given channel with sound + test mode
void rfmod_start(uint8_t channel, rfmod_sound_system sound, bool testMode) {
    
    if (sound > DK_6_5MHZ) sound = BG_5_5MHZ;

    rfmod_setFreqBytes(&fm, &fl, channel, testMode);

    data[0] = 0x88;                        // C0 register value
    data[1] = 0x40 | rfmod_sound[sound];   // C1 register value
    data[2] = fm;
    data[3] = fl;

    swi2c_write_array(RFMOD_ADDR, data, 4);
}

// Sleep mode
void rfmod_sleep(void) {
    data[0] = 0xA8;
    data[1] = 0x20;
    swi2c_write_array(RFMOD_ADDR, data, 2);
}

// change channel while running
void rfmod_changeChannel(uint8_t channel, bool testMode) {

    rfmod_setFreqBytes(&fm, &fl, channel, testMode);

    data[0] = fm;
    data[1] = fl;

    swi2c_write_array(RFMOD_ADDR, data, 2);
}

// sound carrier change
void rfmod_setSoundCarrier(rfmod_sound_system sound) {

    if (sound > DK_6_5MHZ) sound = BG_5_5MHZ;

    data[0] = 0x88;
    data[1] = 0x40 | rfmod_sound[sound];

    swi2c_write_array(RFMOD_ADDR, data, 2);
}

// status byte check
uint8_t rfmod_getStatus(void) {
    status = 0xFF;
    swi2c_read_buf(RFMOD_ADDR, 0x00, &status, 1);
    return status;
}
// experimental free range function through dividers
static void rfmod_setFreqRawBytes(float freqMHz, uint8_t *fm, uint8_t *fl, uint8_t *X) {

    if (freqMHz < 30.0 || freqMHz > 1023.0) {
        freqMHz = 30.0;


    if (freqMHz > 423.0) { divider = 1; *X = 0; }
    else if (freqMHz > 210.0) { divider = 2; *X = 1; }
    else if (freqMHz > 105.0) { divider = 4; *X = 2; }
    else if (freqMHz > 52.0)  { divider = 8; *X = 3; }
    else                      { divider = 16; *X = 4; }

    N = (uint16_t)(freqMHz * 4 * divider);

    *fm = (N >> 6) & 0x3F;
    *fl = (N & 0x3F) << 2;
    *fl |= (*X & 0x03);
}
void rfmod_setFrequencyRaw(float freqMHz, rfmod_sound_system sound, bool testMode) {
		if (sound > DK_6_5MHZ) sound = BG_5_5MHZ;
    rfmod_setFreqRawBytes(freqMHz, &fm, &fl, &X);
    data[0] = 0x88;
		if (X & 0b100) data[0] |= 0x02;
    data[1] = 0x40 | rfmod_sound[sound];
    if (testMode) fm |= 0x40;
    data[2] = fm;
    data[3] = fl;
    swi2c_write_array(RFMOD_ADDR, data, 4);
}
