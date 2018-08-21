#ifndef _HIST_HPP_
#define _HIST_HPP_
#include<stdint.h>

#define PIXEL_OFFSET(X,Y,MAX_COLUMN,CHANNEL) ((X*MAX_COLUMN*CHANNEL)+(Y*CHANNEL))
#define MAX_PIXEL_VALUE 255
#define GRAY_IMG_CHANNEL 1
#define MAX_THRESHOLD_ERROR 1
#pragma pack(1)
struct s_BGR24
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

#pragma pack()

enum e_algorithm
{
    MY_ALGO=0,
    ENTROPY_ALGO,
    OTSU_ALGO,
    FUZZY_MY_ALGO,
    FUZZY_ENTROPY_ALGO,
    FUZZY_OTSU_ALGO
};

#define HISTOGRAM_FUZZY_WINDOW 5

int histogram_fuzzy_weight[HISTOGRAM_FUZZY_WINDOW]=
{
    2,
    3,
    4,
    3,
    2
};

int histogram_fuzzy_index[HISTOGRAM_FUZZY_WINDOW]=
{
    -2,
    -1,
    0,
    1,
    2
};
#endif