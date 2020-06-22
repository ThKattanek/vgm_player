//////////////////////////////////////////////////
//                                              //
// GB DMG emulation                             //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.h                        //
//                                              //
// last changes at 06-22-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef GB_DMGCLASS_H
#define GB_DMGCLASS_H

#include <cstdint>

class GB_DMGClass
{
public:
    GB_DMGClass();
    ~GB_DMGClass();

    void SetClockSpeed(uint32_t clockspeed);
    void SetSampleRate(uint32_t samplerate);
    void WriteReg(uint8_t reg_nr, uint8_t value);
    float GetNextSample();
};

#endif // GB_DMGCLASS_H
