#ifndef _HIST_HPP_
#define _HIST_HPP_
#include<stdint.h>

#define PIXEL_OFFSET(X,Y,MAX_COLUMN,CHANNEL) ((X*MAX_COLUMN*CHANNEL)+(Y*CHANNEL))
#define MAX_PIXEL_VALUE 255
#define GRAY_IMG_CHANNEL 1

#pragma pack(1)
struct s_BGR24
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

#pragma pack()



#endif