#ifndef __BMP_API_H
#define __BMP_API_H
#include<stdint.h>

enum e_bmp_cmdline_args
{
    BMP_ARG_API=0,           /* Index of executable name,bmp_api.o at argv[] */
    BMP_ARG_FILENAME,        /* Index of <FileName> argument at argv[] */
    BMP_ARG_OPERATION,       /* Index of operation; example -f or -c at argv[] */
    BMP_ARG_SUBOPERATION,    /* Index of suboperation example 0 or 1 or 2 for -c at argv[] */
    BMP_ARG_MAX        /* No Argument can have this Index in argv[],argc isn't allowed to go beyond this value */ 
};

/* Valid operation's enum and its corresponding array of pointer to strings */
enum e_bmp_api_operation
{
    BMP_OP_FLIP=0,
    BMP_OP_CONVERT,
    BMP_OP_FILTER,
    BMP_OP_MAX
};

char *bmp_api_operation_str[BMP_OP_MAX]=
{
    "flip",
    "convert",
    "filter"
};


/* Valid "FLIP" suboperation enum and its corresponding array of pointer to strings */
enum e_bmp_api_flip_suboperation{
    BMP_SUBOP_FLIP_LEFT_DIAGONAL=0,
    BMP_SUBOP_FLIP_RIGHT_DIAGONAL,
    BMP_SUBOP_FLIP_MAX
};

char *bmp_api_flip_suboperation_str[BMP_SUBOP_FLIP_MAX]=
{
    "ld",
    "rd"
};

/* Valid "CONVERT" suboperation enum and its corresponding array of pointer to strings */
enum e_bmp_api_convert_suboperation{
    BMP_SUBOP_CONVERT_MAXIMUM=0,
    BMP_SUBOP_CONVERT_MINIMUM,
    BMP_SUBOP_CONVERT_AVERAGE,
    BMP_SUBOP_CONVERT_MAX
};
char *bmp_api_convert_suboperation_str[BMP_SUBOP_CONVERT_MAX]=
{
    "max",
    "min",
    "avg"
};

/* Valid "FILTER" suboperation enum and its corresponding array of pointer to strings */
enum e_bmp_api_filter_suboperation{
    BMP_SUBOP_FILTER_MAXIMUM=0,
    BMP_SUBOP_FILTER_MINIMUM,
    BMP_SUBOP_FILTER_AVERAGE,
    BMP_SUBOP_FILTER_MEDIAN,
    BMP_SUBOP_FILTER_MAX
};
char *bmp_api_filter_suboperation_str[BMP_SUBOP_FILTER_MAX]=
{
    "max",
    "min",
    "avg",
    "median"
};

/** 
 * parsed_cmdline_args structure. This structure gets populated 
 * by bmp_api_validate_cmdline_args(). This structure is then 
 * used by algo block to determine operation and suboperation  
 * requested by the user. 
**/ 
typedef uint32_t t_bmp_api_operation;
typedef uint32_t t_bmp_api_suboperation;

struct
{
t_bmp_api_operation operation;
t_bmp_api_suboperation suboperation;
}parsed_cmdline_args;

/**
 * Currently "filter" operation has maximum number of suboperations 
 * so set BMP_SUBOP_MAX to BMP_SUBOP_FILTER_MAX. This is used to
 * initialize parsed_cmdline_args structure with default values
 * in bmp_api_validate_cmdline_args() function. 
**/ 
#define BMP_SUBOP_MAX BMP_SUBOP_FILTER_MAX

#pragma pack(1)
    struct s_dib_hdr_v3 {
        uint32_t header_sz;      /* DIB Header size should be 40 bytes for Version 3 and 12 for Version 2 */
        uint32_t width;          /* Width of Image in pixels */
        uint32_t height;         /* Height of Image in Pixels */
        uint16_t nplanes;        /* Number of color planes;Don't really know what it's for */
        uint16_t bitspp;         /* Bits per pixel */
        uint32_t compress_type;  /* Compression method, Currently no compression is supported(Value=0) */
        uint32_t bmp_bytesz;     /* Total size of pixel data */
        uint32_t hres;           /* Horizontal resolution in pixels per meter */
        uint32_t vres;           /* Vertical resolution in pixels per meter */
        uint32_t ncolors;        /* Total number of color palette used, 0 means 2^bitspp */
        uint32_t nimpcolors;     /* Number of Important colors used, Don't know what it means */
    };

   struct s_bmp_header {
        uint16_t signature;              /* Should be 0x4d42 or BM in ASCII, OS identifier */
        uint32_t filesize;               /* Size of file including Header */
        uint32_t reserved;               /* Reserved;Duh! */
        uint32_t dataoffset;             /* offset from where bitmap data is located */
        struct s_dib_hdr_v3 dib_hdr;     /* DIB version 3 header */
    };

    struct s_RGB24
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };

    struct s_colortable_row
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t index;
    };


#define GET_PIXEL_SIZE(_BMP_HDR_PTR_) (((_BMP_HDR_PTR_)->dib_hdr.bitspp)/8)
#define GET_HEIGHT(_BMP_HDR_PTR_) ((_BMP_HDR_PTR_)->dib_hdr.height)
#define GET_WIDTH(_BMP_HDR_PTR_) ((_BMP_HDR_PTR_)->dib_hdr.width)
#define GET_COMPRESSION(_BMP_HDR_PTR_) ((_BMP_HDR_PTR_)->dib_hdr.compress_type)
#define GET_PADDING(_BMP_HDR_PTR_) (( (0==(GET_WIDTH(_BMP_HDR_PTR_)*GET_PIXEL_SIZE(_BMP_HDR_PTR_))%4)? 0: (4-((GET_WIDTH(_BMP_HDR_PTR_)*GET_PIXEL_SIZE(_BMP_HDR_PTR_))%4))))
#define GET_PIXEL_OFFSET(_BMP_HDR_PTR_,X,Y) ((((GET_PIXEL_SIZE(_BMP_HDR_PTR_)*GET_WIDTH(_BMP_HDR_PTR_))+GET_PADDING(_BMP_HDR_PTR_))*Y)+(X*GET_PIXEL_SIZE(_BMP_HDR_PTR_)))
#define GET_RAW_BMP_SIZE(_BMP_HDR_PTR_) GET_PIXEL_OFFSET(_BMP_HDR_PTR_,0,GET_WIDTH(_BMP_HDR_PTR_))
#define GET_BMP_DATAOFFSET(_BMP_HDR_PTR_) ((_BMP_HDR_PTR_)->dataoffset)
#define GET_COLORTABLE_SIZE(_BMP_HDR_PTR_) ((1<<((_BMP_HDR_PTR_)->dib_hdr.bitspp))*sizeof(struct s_colortable_row))
/**
 * Just an efficient version of GET_PIXEL_OFFSET
**/
#define GET_PIXEL_OFFSET_FAST(_BMP_HDR_PTR_,X,Y,_PADDING_) ((((GET_PIXEL_SIZE(_BMP_HDR_PTR_)*GET_WIDTH(_BMP_HDR_PTR_))+_PADDING_)*Y)+X*GET_PIXEL_SIZE(_BMP_HDR_PTR_))

#define CHUNK_SIZE 1024
#define GRAY_SCALE_PIXEL_SIZE 1
#define BMP_V3_SIGNATURE 0x4d42
#define BMP_V3_DIB 40
/* Filter Specs */
#define F_WIDTH 3
#define F_HEIGHT 3
#define F_LWIDTH -1
#define F_HWIDTH 1
#define F_LHEIGHT -1
#define F_HHEIGHT 1
#define BOUNDARY_PIXEL_FILTER_PAD 127

    enum e_bmp_compression_method {
        BI_RGB = 0,
        BI_RLE8,
        BI_RLE4,
        BI_BITFIELDS,
        BI_JPEG,
        BI_PNG,
        BI_MAX
        };

    char * bmp_compression_method_str[BI_MAX]={
        "None",
        "RLE 8-bit/pixel",
        "RLE 4-bit/pixel",
        "Bit field",
        "JPEG",
        "PNG"
    };




/* To make output more beautiful */
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#endif