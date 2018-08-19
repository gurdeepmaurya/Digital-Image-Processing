

#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <errno.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>

#include <time.h>

#include "bmp_api.h"
void bmp_api_usage()
{
    printf(
    BOLDWHITE
    "\nCorrect Usage is:./bmp_api.0 <ImageName> [<operation> [<suboperation>]]"
    "\nIf an operation is provided then it's mandatory to provide suboperation."
    "\nList of valid <operation>    <suboperations>"
    "\n                 flip            ld"
    "\n                 flip            rd"
    "\n-------------------------------------------"
    "\n                convert          max"
    "\n                convert          min"
    "\n                convert          avg"
    "\n-------------------------------------------"
    "\n                filter           max"
    "\n                filter           min"
    "\n                filter           avg"
    "\n                filter          median\n"
    RESET);

}





/* This function Validates user arguments */
bool bmp_api_validate_cmdline_args(int argc, char * argv[])
{
    bool args_valid=true;
    bool file_exists=((0==access(argv[BMP_ARG_FILENAME],F_OK)? true:false));
    parsed_cmdline_args.operation=BMP_OP_MAX;
    parsed_cmdline_args.suboperation=BMP_SUBOP_MAX;

    if(argc<(BMP_ARG_FILENAME+1)||argc>BMP_ARG_MAX) /* This +1 is for name of executable argument */
    { 
    /* Nothing has been supplied at Args or too many arguments*/
    args_valid=false;
    }
    else if(false == file_exists) 
    {
        /*File Does not exist*/
        printf(BOLDMAGENTA"\nFile %s does not exist!"RESET,argv[BMP_ARG_FILENAME]);
        args_valid=false;
    }/* Validation check for existence of file ends here */
    else if((argc==2)&&(true ==file_exists))
    {
        /* User has only supplied file name, He wishes to read BMP file header only */
        args_valid=true;
    } 
    else if((argc==4)&&(0==strcmp(argv[BMP_ARG_OPERATION],bmp_api_operation_str[BMP_OP_FLIP])))
    {
        /* User wants to flip the Image */
        if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_flip_suboperation_str[BMP_SUBOP_FLIP_LEFT_DIAGONAL]))
        {
            /* User wants to flip the Image along left diagonal */
            parsed_cmdline_args.operation=BMP_OP_FLIP;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FLIP_LEFT_DIAGONAL;

        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_flip_suboperation_str[BMP_SUBOP_FLIP_RIGHT_DIAGONAL])) 
        {
            /* User wants to flip the Image along right diagonal */
            parsed_cmdline_args.operation=BMP_OP_FLIP;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FLIP_RIGHT_DIAGONAL;

        }
        else
        {

        }
    }/* Validation check for flip operation end here  */
    else if((argc==4)&&(0==strcmp(argv[BMP_ARG_OPERATION],bmp_api_operation_str[BMP_OP_CONVERT])))
    {
        /* User wants to convert colored BMP file to gray scale */
        if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_convert_suboperation_str[BMP_SUBOP_CONVERT_MAXIMUM]))
        {
            /* Conversion to gray scale with Max of RGB suboperation */

            parsed_cmdline_args.operation=BMP_OP_CONVERT;
            parsed_cmdline_args.suboperation=BMP_SUBOP_CONVERT_MAXIMUM;

        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_convert_suboperation_str[BMP_SUBOP_CONVERT_MINIMUM])) 
        {
            /* Conversion to gray scale with Minimum of RGB suboperation */

            parsed_cmdline_args.operation=BMP_OP_CONVERT;
            parsed_cmdline_args.suboperation=BMP_SUBOP_CONVERT_MINIMUM;

        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_convert_suboperation_str[BMP_SUBOP_CONVERT_AVERAGE])) 
        {
            /* Conversion to gray scale with Average of RGB suboperation */
            parsed_cmdline_args.operation=BMP_OP_CONVERT;
            parsed_cmdline_args.suboperation=BMP_SUBOP_CONVERT_AVERAGE;
        }
        else
        {
            /* Uh-oh! Wrong suboperation for conversion operation provided  */
            args_valid=false;
        }
    }/* Validation check for conversion operation end here */
    else if((argc==4)&&(0==strcmp(argv[BMP_ARG_OPERATION],bmp_api_operation_str[BMP_OP_FILTER])))
    {
        /* User wants to apply filter to BMP file */
        if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_filter_suboperation_str[BMP_SUBOP_FILTER_MAXIMUM]))
        {
            /* Application of filter with Max of 3x3 pixel grid suboperation */
            parsed_cmdline_args.operation=BMP_OP_FILTER;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FILTER_MAXIMUM;
        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_filter_suboperation_str[BMP_SUBOP_FILTER_MINIMUM])) 
        {
            /* Application of filter with Minimum of 3x3 pixel grid suboperation */
            parsed_cmdline_args.operation=BMP_OP_FILTER;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FILTER_MINIMUM;

        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_filter_suboperation_str[BMP_SUBOP_FILTER_AVERAGE])) 
        {
            /* Application of filter with Average of 3x3 pixel grid suboperation */
            parsed_cmdline_args.operation=BMP_OP_FILTER;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FILTER_AVERAGE;

        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_filter_suboperation_str[BMP_SUBOP_FILTER_MEDIAN])) 
        {
            /* Application of filter with Median of 3x3 pixel grid suboperation */
            parsed_cmdline_args.operation=BMP_OP_FILTER;
            parsed_cmdline_args.suboperation=BMP_SUBOP_FILTER_MEDIAN;

        }
        else
        {
            /* Uh-oh! Wrong suboperation for filter operation provided  */
            args_valid=false;
        }
    }/* Validation check for Filter Operation end here */
    else
    {
        /* Covers rest of the invalid cases */
        args_valid=false;
    }

    return (args_valid);
}



/* This function displays header info */

void bmp_api_display_header_info(const struct s_bmp_header const * bmp_hdr_src_ptr)
{
    /* Null pointer checks */
    if(bmp_hdr_src_ptr==NULL)
    {
        printf(BOLDRED"\nError:Null ptr passed"RESET);
    }
    else{
        printf( "BMP Header Signature=%x\n"
                "BMP Header FileSize=%d\n"
                "BMP Header offset=%d\n"
                "DIB Header Size:%d\n"
                "Image Width:%u\n"
                "Image Height:%u\n"
                "Number of planes:%u\n"
                "Bits per pixels:%u\n"
                "Compression Method:%d [%s]\n"
                "Pixel data Size:%d\n"
                "Horizontal resolution in pixel per meter:%d\n"
                "Vertical resolution in pixel per meter:%d\n"
                "Numbers of colors in color palette:%u [0 means %lu]\n"
                "Number of Important colors:%d [0 means %lu]\n",
                bmp_hdr_src_ptr->signature,
                bmp_hdr_src_ptr->filesize,
                bmp_hdr_src_ptr->dataoffset,
                bmp_hdr_src_ptr->dib_hdr.header_sz,
                bmp_hdr_src_ptr->dib_hdr.width,
                bmp_hdr_src_ptr->dib_hdr.height,
                bmp_hdr_src_ptr->dib_hdr.nplanes,
                bmp_hdr_src_ptr->dib_hdr.bitspp,

                bmp_hdr_src_ptr->dib_hdr.compress_type,
                (((bmp_hdr_src_ptr->dib_hdr.compress_type>=BI_MAX)||(bmp_hdr_src_ptr->dib_hdr.compress_type<BI_RGB)))?"Unknown":bmp_compression_method_str[bmp_hdr_src_ptr->dib_hdr.compress_type],
                    
                bmp_hdr_src_ptr->dib_hdr.bmp_bytesz,
                bmp_hdr_src_ptr->dib_hdr.hres,
                bmp_hdr_src_ptr->dib_hdr.vres,

                bmp_hdr_src_ptr->dib_hdr.ncolors,
                (unsigned long int)(1<<bmp_hdr_src_ptr->dib_hdr.bitspp),

                bmp_hdr_src_ptr->dib_hdr.nimpcolors,
                (unsigned long int) (1<<bmp_hdr_src_ptr->dib_hdr.bitspp));

                /* Debug code MACROS */
                printf("\n <MACROS TESTING>"
                        "\nGET_PIXEL_SIZE(_BMP_HDR_SRC_PTR_):%d"
                        "\nGET_HEIGHT(_BMP_HDR_SRC_PTR_):%d"
                        "\nGET_WIDTH(_BMP_HDR_SRC_PTR_):%d"
                        "\nGET_PADDING(_BMP_HDR_SRC_PTR_):%d"
                        "\nGET_PIXEL_OFFSET(_BMP_HDR_SRC_PTR_,X,Y):%d"
                        "\nGET_RAW_BMP_SIZE(_BMP_HDR_SRC_PTR_):%d"
                        "\nGET_BMP_DATAOFFSET(_BMP_HDR_SRC_PTR_):%d\n\n",
                        GET_PIXEL_SIZE(bmp_hdr_src_ptr),
                        GET_HEIGHT(bmp_hdr_src_ptr),
                        GET_WIDTH(bmp_hdr_src_ptr),
                        GET_PADDING(bmp_hdr_src_ptr),
                        GET_PIXEL_OFFSET(bmp_hdr_src_ptr,1,0),
                        GET_RAW_BMP_SIZE(bmp_hdr_src_ptr),
                        GET_BMP_DATAOFFSET(bmp_hdr_src_ptr));
    }
}
/**
 * BMP file will have V3 header if signature is equale to 0x4d42 and 
 * size of dib header is 40 bytes
**/
bool bmp_api_validate_header_operation(const struct s_bmp_header const * bmp_hdr_src_ptr)
{
    bool retvalue = false;
    
    if((bmp_hdr_src_ptr->signature==BMP_V3_SIGNATURE)&&(bmp_hdr_src_ptr->dib_hdr.header_sz==BMP_V3_DIB))
    {
        if((parsed_cmdline_args.operation==BMP_OP_FLIP)&&(BI_RGB!=GET_COMPRESSION(bmp_hdr_src_ptr)))
        {
            printf(BOLDMAGENTA "\nError:Input file should have compression type 'none'(0) for flip operation" RESET);
        }
        else if((parsed_cmdline_args.operation == BMP_OP_CONVERT)&&((sizeof(struct s_RGB24)!=GET_PIXEL_SIZE(bmp_hdr_src_ptr))||(BI_RGB!=GET_COMPRESSION(bmp_hdr_src_ptr))))
        {
            printf(BOLDMAGENTA "\nError:Input file should be 24 bit RGB and have compression type 'none'(0) for CONVERT operation" RESET);
        }
        else if((parsed_cmdline_args.operation == BMP_OP_FILTER)&&((GRAY_SCALE_PIXEL_SIZE!=GET_PIXEL_SIZE(bmp_hdr_src_ptr))||(BI_RGB!=GET_COMPRESSION(bmp_hdr_src_ptr))))
        {
            printf(BOLDMAGENTA "\nError:Input file should be grayscale and have compression type 'none'(0) for FILTER operation\n" RESET);
        }
        else
        {
            retvalue=true;
            printf(BOLDYELLOW "\nHeader and Operation validation passed\n"RESET);
        }
    }
    else
    {
    printf(BOLDMAGENTA "\nError:BMP files with v3 header is supported" RESET);
    }

    return(retvalue);
}

/**
 * This function reads the input image and flips it 
 * along right diagonal and writes it to an output
 * file DDMMYYYY_HHHHSS.bmp
**/ 
void bmp_compose_output_file_name(char *outfilename,char *custom_string)
{
 
time_t t = time(NULL);
struct tm tm = *localtime(&t);

sprintf(outfilename,"%s_%d-%d-%d_%d-%d-%d.bmp",custom_string, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}



/**
 * This function performs the actual flipping operation
**/


void bmp_api_flip_along_right_diagonal(int fs,const struct s_bmp_header const * bmp_hdr_src_ptr)
{
int fd;/* Destination file descriptor */
char outfilename[50]={0}; /* Destination file name */


/* Variables to keep track of pixel position */
uint32_t x=0,y=0;
struct s_bmp_header bmp_hdr_dstn={0}; /* Destination file header */

/* Copy source file header into destination file header */
bmp_hdr_dstn=*bmp_hdr_src_ptr;
bmp_hdr_dstn.dib_hdr.height=GET_WIDTH(bmp_hdr_src_ptr);
bmp_hdr_dstn.dib_hdr.width=GET_HEIGHT(bmp_hdr_src_ptr);
uint32_t s_padding= GET_PADDING(bmp_hdr_src_ptr),
         d_padding= GET_PADDING(&bmp_hdr_dstn),
         s_offset=0,
         d_offset=0;



ssize_t pixel_data=GET_RAW_BMP_SIZE(bmp_hdr_src_ptr),
        pixel_size=GET_PIXEL_SIZE(bmp_hdr_src_ptr),
        data_written=0;
ssize_t chunk_size=(pixel_data<CHUNK_SIZE)?pixel_data:CHUNK_SIZE;
int chunk_written=0;
uint8_t * s_buffer= (uint8_t *) malloc(pixel_data);
uint8_t * d_buffer= (uint8_t *) malloc(pixel_data);

bmp_compose_output_file_name(outfilename,"FLIPPED");
outfilename[49]='\0';

if(NULL==s_buffer||NULL==d_buffer)
{
    printf(BOLDRED"Error:Cant allocate buffer"RESET);
}
else
{
    memset((void *) s_buffer,0,pixel_data);
    memset((void *) d_buffer,0,pixel_data);
    /* Rewind the offset to beginning of BMP data in source file */
    if(GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)!=lseek(fs,GET_BMP_DATAOFFSET(bmp_hdr_src_ptr),SEEK_SET))
    {
        printf(BOLDRED"\nError:Can't set cursor in source file"RESET);
    }
    else
    {
        /* Copy entire pixel data in source buffer */
        if(pixel_data!=read(fs,s_buffer,pixel_data))
        {
            /*Problem in reading source file */
            printf(BOLDRED"\nError:Can't read pixel data from source file"RESET);
        }
        else
        {
            for(x=0;x<GET_WIDTH(bmp_hdr_src_ptr);x++)
            {
                for(y=0;y<GET_HEIGHT(bmp_hdr_src_ptr);y++)
                {
                    /* Get pixel offset from source pixel */
                    s_offset=GET_PIXEL_OFFSET_FAST(bmp_hdr_src_ptr,x,y,s_padding);
                    /* Get pixel offset from destination pixel */
                    d_offset=GET_PIXEL_OFFSET_FAST(&bmp_hdr_dstn,y,x,d_padding);
                    memcpy((void *) (d_buffer+d_offset),(void *) (s_buffer+s_offset),pixel_size);
                }
            }
                        
            fd=open(outfilename,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);
            if(fd==-1)
            {
                printf(BOLDRED"\nError in creating destination file"RESET);
            }
            else
            {
                if(sizeof(bmp_hdr_dstn)!=write(fd,&bmp_hdr_dstn,sizeof(bmp_hdr_dstn)))
                {
                    printf(BOLDRED"\nError in writing BMP header into destination file"RESET);
                }
                else
                {
                    /* Need to add code that writes colortable for grayscale here */
                    if(pixel_size == GRAY_SCALE_PIXEL_SIZE)
                    {
                        /* Copy color table,Set cursor at start of color table */
                        if(sizeof(struct s_bmp_header)!=lseek(fs,sizeof(struct s_bmp_header),SEEK_SET))
                        {
                            printf(BOLDRED"\nError in setting cursor offset in source grayscale image"RESET);
                        }
                        else
                        {
                            ssize_t colortable_size=(ssize_t)GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)-sizeof(struct s_bmp_header),colortabledata_rw=0;
                            uint8_t temp;
                            while(colortabledata_rw!=colortable_size)
                            {
                                if(sizeof(uint8_t)!=read(fs,&temp,sizeof(uint8_t)))
                                {
                                    printf(BOLDRED"\nError in reading colortable from source grayscale image"RESET);
                                    break;
                                }
                                else
                                {
                                    if(sizeof(uint8_t)!=write(fd,&temp,sizeof(uint8_t)))
                                    {
                                        printf(BOLDRED"\nError in writing colortable to destination grayscale image"RESET);
                                        break;
                                    }
                                    else
                                    {
                                        colortabledata_rw++;
                                    }
                                }
                                
                            }
                            
                            

                        }

                    }/* Color table write ends */
                    /* Write BMP DATA */
                    while(data_written!=pixel_data)
                    {
                        chunk_written=write(fd,(d_buffer+data_written),chunk_size);
                        if(chunk_written==-1)
                        {
                        printf(BOLDRED"\nError in writing BMP pixel data into destination file:%ld,%s\n"RESET,data_written,strerror(errno));
                        break;
                        }
                        else
                        {
                            data_written+=chunk_written;
                            if((pixel_data-data_written)<chunk_size)
                            {
                            chunk_size=pixel_data-data_written;
                            }
                        }

                
                    }
                    if(data_written==pixel_data)
                    {
                        printf(BOLDGREEN"\nOutput file %s generated\n"RESET,outfilename);
                    }
                    else
                    {
                        printf(BOLDRED"\nOperation failed please delete generated file %s"RESET,outfilename);
                    }
                }
                
            close(fd);
            }

        }
        
    }

free(s_buffer);
free(d_buffer);
}

}

/**
 * Function to calculate Max of RGB
**/
uint8_t bmp_RGB24_maximum(struct s_RGB24 * RGB24_ptr)
{
    uint8_t maxvalue;
    if(RGB24_ptr->blue>RGB24_ptr->green)
    {
        if(RGB24_ptr->blue>RGB24_ptr->red)
        {
            maxvalue=RGB24_ptr->blue;
        }
        else
        {
            maxvalue=RGB24_ptr->red;
        }
    }
    else
    {
        if(RGB24_ptr->green>RGB24_ptr->red)
        {
            maxvalue=RGB24_ptr->green;
        }
        else
        {
            maxvalue=RGB24_ptr->red;
        }
    }
    return maxvalue;
}


/**
 * Function to calculate min of RGB
**/
uint8_t bmp_RGB24_minimum(struct s_RGB24 * RGB24_ptr)
{
    uint8_t minvalue;
    if(RGB24_ptr->blue<RGB24_ptr->green)
    {
        if(RGB24_ptr->blue<RGB24_ptr->red)
        {
            minvalue=RGB24_ptr->blue;
        }
        else
        {
            minvalue=RGB24_ptr->red;
        }
    }
    else
    {
        if(RGB24_ptr->green<RGB24_ptr->red)
        {
            minvalue=RGB24_ptr->green;
        }
        else
        {
            minvalue=RGB24_ptr->red;
        }
    }
    return minvalue;
}


/**
 * Function to calculate average of RGB
**/
uint8_t bmp_RGB24_average(struct s_RGB24 * RGB24_ptr)
{
    uint16_t average;
    average=(RGB24_ptr->blue+RGB24_ptr->green+RGB24_ptr->red)/3;
    return ((uint8_t)average);
}

/**
 * Convert to gray scale function,A lot of it is being reused from
 * bmp_api_flip_along_rd()
**/
void bmp_api_convert(int fs,const struct s_bmp_header const * bmp_hdr_src_ptr)
{
int fd;/* Destination file descriptor */
char outfilename[50]={0}; /* Destination file name */


/* Variables to keep track of pixel position */
uint32_t x=0,y=0;
struct s_bmp_header bmp_hdr_dstn={0}; /* Destination file header */

/* Copy source file header into destination file header */
bmp_hdr_dstn=*bmp_hdr_src_ptr;

/* Code Deviates here a bit*/

bmp_hdr_dstn.dib_hdr.bitspp/=3;
bmp_hdr_dstn.dib_hdr.bmp_bytesz=GET_RAW_BMP_SIZE(&bmp_hdr_dstn);
GET_BMP_DATAOFFSET(&bmp_hdr_dstn)= GET_COLORTABLE_SIZE(&bmp_hdr_dstn)+sizeof(bmp_hdr_dstn);
bmp_hdr_dstn.filesize=GET_BMP_DATAOFFSET(&bmp_hdr_dstn)+bmp_hdr_dstn.dib_hdr.bmp_bytesz;
uint32_t s_padding= GET_PADDING(bmp_hdr_src_ptr),
         d_padding= GET_PADDING(&bmp_hdr_dstn),
         s_offset=0,
         d_offset=0;



ssize_t s_pixel_data=GET_RAW_BMP_SIZE(bmp_hdr_src_ptr),
        d_pixel_data=GET_RAW_BMP_SIZE(&bmp_hdr_dstn),
        data_written=0;
ssize_t chunk_size=(d_pixel_data<CHUNK_SIZE)?d_pixel_data:CHUNK_SIZE;
int chunk_written=0;
uint8_t * s_buffer= (uint8_t *) malloc(s_pixel_data);
uint8_t * d_buffer= (uint8_t *) malloc(d_pixel_data);

bmp_compose_output_file_name(outfilename,"GRAYSCALE");
outfilename[49]='\0';

if(NULL==s_buffer||NULL==d_buffer)
{
    printf(BOLDRED"Error:Cant allocate buffer"RESET);
}
else
{
    memset((void *) s_buffer,0,s_pixel_data);
    memset((void *) d_buffer,0,d_pixel_data);
    /* Rewind the offset to beginning of BMP data in source file */
    if(GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)!=lseek(fs,GET_BMP_DATAOFFSET(bmp_hdr_src_ptr),SEEK_SET))
    {
        printf(BOLDRED"\nError:Can't set cursor in source file"RESET);
    }
    else
    {
        /* Copy entire pixel data in source buffer */
        if(s_pixel_data!=read(fs,s_buffer,s_pixel_data))
        {
            /*Problem in reading source file */
            printf(BOLDRED"\nError:Can't read pixel data from source file"RESET);
        }
        else
        {
            for(x=0;x<GET_WIDTH(bmp_hdr_src_ptr);x++)
            {
                for(y=0;y<GET_HEIGHT(bmp_hdr_src_ptr);y++)
                {
                    /* Get pixel offset from source pixel */
                    s_offset=GET_PIXEL_OFFSET_FAST(bmp_hdr_src_ptr,x,y,s_padding);
                    /* Get pixel offset from destination pixel */
                    d_offset=GET_PIXEL_OFFSET_FAST(&bmp_hdr_dstn,x,y,d_padding);
                    if(parsed_cmdline_args.suboperation == BMP_SUBOP_CONVERT_MAXIMUM)
                    {
                        *(d_buffer+d_offset)=bmp_RGB24_maximum((struct s_RGB24 *)(s_buffer+s_offset));
                    }
                    else if(parsed_cmdline_args.suboperation == BMP_SUBOP_CONVERT_MINIMUM)
                    {
                        *(d_buffer+d_offset)=bmp_RGB24_minimum((struct s_RGB24 *)(s_buffer+s_offset));
                    }
                    else if(parsed_cmdline_args.suboperation == BMP_SUBOP_CONVERT_AVERAGE)
                    {
                        *(d_buffer+d_offset)=bmp_RGB24_average((struct s_RGB24 *)(s_buffer+s_offset));
                    }
                    else
                    {
                        printf(BOLDRED"\nError:Cannot determine grayscale conversion suboperation\n"RESET);
                    }

                }
            }
                        
            fd=open(outfilename,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);
            if(fd==-1)
            {
                printf(BOLDRED"\nError in creating destination file"RESET);
            }
            else
            {
                if(sizeof(bmp_hdr_dstn)!=write(fd,&bmp_hdr_dstn,sizeof(bmp_hdr_dstn)))
                {
                    printf(BOLDRED"\nError in writing BMP header into destination file"RESET);
                }
                else
                {
                    /* Need to add code that writes colortable for grayscale here */
                    /* Composing and writing color table on the go */
                    uint16_t temp=0;
                    struct s_colortable_row color_table_row={0};
                    ssize_t colortable_size=(ssize_t)GET_COLORTABLE_SIZE(&bmp_hdr_dstn),
                            colortabledata_rw=0,
                            colortable_row_size=sizeof(struct s_colortable_row);

                    while(colortabledata_rw!=colortable_size)
                    {
                        color_table_row.blue=temp;
                        color_table_row.green=temp;
                        color_table_row.red=temp;
                        color_table_row.index=temp;
                        if(colortable_row_size!=write(fd,&color_table_row,colortable_row_size))
                        {
                            printf(BOLDRED"\nError in writing colortable to destination grayscale image"RESET);
                            break;
                        }
                        else
                        {
                            colortabledata_rw+=colortable_row_size;
                            temp++;
                        }
                    }/* Color table write ends */

                    /* Write BMP DATA */
                    while(data_written!=d_pixel_data)
                    {
                        chunk_written=write(fd,(d_buffer+data_written),chunk_size);
                        if(chunk_written==-1)
                        {
                        printf(BOLDRED"\nError in writing BMP pixel data into destination file:%ld,%s\n"RESET,data_written,strerror(errno));
                        break;
                        }
                        else
                        {
                            data_written+=chunk_written;
                            if((d_pixel_data-data_written)<chunk_size)
                            {
                            chunk_size=d_pixel_data-data_written;
                            }
                        }
                
                    }
                    if(data_written==d_pixel_data)
                    {
                        printf(BOLDGREEN"\nOutput file %s generated\n"RESET,outfilename);
                    }
                    else
                    {
                        printf(BOLDRED"\nOperation failed please delete generated file %s"RESET,outfilename);
                    }
                }
                
            close(fd);
            }

        }
        
    }

free(s_buffer);
free(d_buffer);
}

}

/* Ascending order bubble sort */
void bmp_bubble_sort(uint8_t * a)
{
    uint8_t i,j,temp,upper_bound=F_WIDTH*F_HEIGHT;
for(i=1;i<upper_bound;i++)
{
    for(j=0;j<(upper_bound-i);j++)
    {
        if(a[j]>a[j+1])
        {
            temp=a[j];
            a[j]=a[j+1];
            a[j+1]=temp;
        }
    }
}

}

uint8_t bmp_filter_subop(uint8_t *a)
{
    uint16_t retval=0;
    uint8_t i=0,upper_bound=F_WIDTH*F_HEIGHT;
    if(parsed_cmdline_args.suboperation == BMP_SUBOP_FILTER_AVERAGE)
    {
        for(i=0;i<upper_bound;i++)
        {
            retval+=a[i];
        }
        retval=retval/(upper_bound);
    }
    else if(parsed_cmdline_args.suboperation == BMP_SUBOP_FILTER_MINIMUM)
    {
        bmp_bubble_sort(a);
        retval=a[0];
    }
    else if(parsed_cmdline_args.suboperation == BMP_SUBOP_FILTER_MAXIMUM)
    {
        bmp_bubble_sort(a);
        retval=a[upper_bound-1];
    }
    else if(parsed_cmdline_args.suboperation == BMP_SUBOP_FILTER_MEDIAN)
    {
        bmp_bubble_sort(a);
        if(upper_bound%2)
        {
            retval=a[upper_bound/2];
        }
        else
        {
            retval=(a[upper_bound/2]+a[(upper_bound/2)-1])/2;
        }  
    }
    else
    {
        retval=0;
    }
return ((uint8_t)(retval));
}


/**
 * This function performs the spatial filtering operation using 3x3 window size
**/


void bmp_api_filter(int fs,const struct s_bmp_header const * bmp_hdr_src_ptr)
{

int fd;/* Destination file descriptor */
char outfilename[50]={0}; /* Destination file name */


/* Variables to keep track of pixel position */
uint32_t x=0,y=0;
/* variables for filter width */
int8_t f_x=0,f_y=0;
struct s_bmp_header bmp_hdr_dstn={0}; /* Destination file header */

/* Copy source file header into destination file header */
bmp_hdr_dstn=*bmp_hdr_src_ptr;

uint32_t s_padding= GET_PADDING(bmp_hdr_src_ptr),
         d_padding= GET_PADDING(&bmp_hdr_dstn),
         s_offset=0,
         d_offset=0;



ssize_t pixel_data=GET_RAW_BMP_SIZE(bmp_hdr_src_ptr),
        pixel_size=GET_PIXEL_SIZE(bmp_hdr_src_ptr),
        data_written=0;
ssize_t chunk_size=(pixel_data<CHUNK_SIZE)?pixel_data:CHUNK_SIZE;
int chunk_written=0;
uint8_t * s_buffer= (uint8_t *) malloc(pixel_data);
uint8_t * d_buffer= (uint8_t *) malloc(pixel_data);

uint8_t a[F_WIDTH*F_HEIGHT],upper_bound=F_WIDTH*F_HEIGHT;
uint8_t temp=0,pixel_value;

bmp_compose_output_file_name(outfilename,"FILTER");

outfilename[49]='\0';

if(NULL==s_buffer||NULL==d_buffer)
{
    printf(BOLDRED"\nError:Can't allocate buffer\n"RESET);
}
else
{

    memset((void *) s_buffer,0,pixel_data);
    memset((void *) d_buffer,0,pixel_data);

    /* Rewind the offset to beginning of BMP data in source file */
    if(GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)!=lseek(fs,GET_BMP_DATAOFFSET(bmp_hdr_src_ptr),SEEK_SET))
    {
        printf(BOLDRED"\nError:Can't set cursor in source file\n"RESET);
    }
    else
    {
        /* Copy entire pixel data in source buffer */
        if(pixel_data!=read(fs,s_buffer,pixel_data))
        {
            /* Problem in reading source file */
            printf(BOLDRED"\nError:Can't read pixel data from source file\n"RESET);
        }
        else
        {
            
            for(x=0;x<GET_WIDTH(bmp_hdr_src_ptr);x++)
            {
                for(y=0;y<GET_HEIGHT(bmp_hdr_src_ptr);y++)
                {
                    temp=0;
                    for(f_x=F_LWIDTH;f_x<=F_HWIDTH;f_x++)
                    {
                        for(f_y=F_LHEIGHT;f_y<=F_HHEIGHT;f_y++)
                        {
                            if((((int64_t)x+f_x)<0)||(((int64_t)y+f_y)<0))
                            { /* This code is for boundary pixels*/
                                a[temp]=BOUNDARY_PIXEL_FILTER_PAD;
                            }
                            else
                            {
                            /* Get pixel offset from source pixel */
                            s_offset=GET_PIXEL_OFFSET_FAST(bmp_hdr_src_ptr,(x+f_x),(y+f_y),s_padding);

                            a[temp]= (uint8_t ) *(s_buffer+s_offset);

                            }
                            temp=((temp<upper_bound)?(temp+1):temp);
                        }
                    }/* We've copied all the neighbouring pixels in a[] */
                    
                    /* At this point we have got all the 3X3 pixels in a linear array*/
                    
                    pixel_value = bmp_filter_subop(a);
                    
                    /* Get pixel offset from destination pixel */
                    d_offset=GET_PIXEL_OFFSET_FAST(&bmp_hdr_dstn,x,y,d_padding);
                    memcpy((void *) (d_buffer+d_offset),(void *) (&pixel_value),pixel_size);
                }
            }
                        
            fd=open(outfilename,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);
            if(fd==-1)
            {
                printf(BOLDRED"\nError in creating destination file"RESET);
            }
            else
            {
                if(sizeof(bmp_hdr_dstn)!=write(fd,&bmp_hdr_dstn,sizeof(bmp_hdr_dstn)))
                {
                    printf(BOLDRED"\nError in writing BMP header into destination file\n"RESET);
                }
                else
                {
                    /* Need to add code that writes colortable for grayscale here */
                    if(pixel_size == GRAY_SCALE_PIXEL_SIZE)
                    {
                        /* Copy color table,Set cursor at start of color table */
                        if(sizeof(struct s_bmp_header)!=lseek(fs,sizeof(struct s_bmp_header),SEEK_SET))
                        {
                            printf(BOLDRED"\nError in setting cursor offset in source grayscale image"RESET);
                        }
                        else
                        {
                            ssize_t colortable_size=(ssize_t)GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)-sizeof(struct s_bmp_header),colortabledata_rw=0;

                            while(colortabledata_rw!=colortable_size)
                            {
                                if(sizeof(uint8_t)!=read(fs,&temp,sizeof(uint8_t)))
                                {
                                    printf(BOLDRED"\nError in reading colortable from source grayscale image"RESET);
                                    break;
                                }
                                else
                                {
                                    if(sizeof(uint8_t)!=write(fd,&temp,sizeof(uint8_t)))
                                    {
                                        printf(BOLDRED"\nError in writing colortable to destination grayscale image"RESET);
                                        break;
                                    }
                                    else
                                    {
                                        colortabledata_rw++;
                                    }
                                }
                                
                            }
                            
                            

                        }

                    }/* Color table write ends */
                    /* Write BMP DATA */
                    while(data_written!=pixel_data)
                    {
                        chunk_written=write(fd,(d_buffer+data_written),chunk_size);
                        if(chunk_written==-1)
                        {
                        printf(BOLDRED"\nError in writing BMP pixel data into destination file:%ld,%s\n"RESET,data_written,strerror(errno));
                        break;
                        }
                        else
                        {
                            data_written+=chunk_written;
                            if((pixel_data-data_written)<chunk_size)
                            {
                            chunk_size=pixel_data-data_written;
                            }
                        }

                
                    }
                    if(data_written==pixel_data)
                    {
                        printf(BOLDGREEN"\nOutput file %s generated\n"RESET,outfilename);
                    }
                    else
                    {
                        printf(BOLDRED"\nOperation failed please delete generated file %s"RESET,outfilename);
                    }
                }
                
            close(fd);
            }

        }
        
    }

free(s_buffer);
free(d_buffer);
}

}






int main(int argc,char *argv[])
{
    int fs; /* Source file Pointer */
    struct s_bmp_header bmp_hdr_src={0}; /* BMP source Header */
    ssize_t bytes_read=0; /* Keeps count of number of bytes read */
    if(false==bmp_api_validate_cmdline_args(argc,argv))
    { 
        bmp_api_usage();
    }
    else
    {
        printf(BOLDGREEN"\nArguments validation passed,performing the requested operation"RESET);
        fs= open(argv[BMP_ARG_FILENAME],O_RDONLY);

        if(fs==-1){
            printf(BOLDMAGENTA "\nError in opening file!!" RESET);
        }
        else{
            bytes_read=read(fs,&bmp_hdr_src,sizeof(struct s_bmp_header));
            if(bytes_read!=sizeof(struct s_bmp_header)){
                printf(BOLDMAGENTA"\nError in reading the file"RESET);
            }
            else if(false==bmp_api_validate_header_operation((const struct s_bmp_header const *) &bmp_hdr_src))
            {
                /* Validate BMP V3 header failed */
                printf(BOLDMAGENTA "\nExiting with Failiure\n" RESET);
                
            }
            else
            {
                /* Display filename info */
                printf("\n\nName of BMP File: %s\n",argv[BMP_ARG_FILENAME]);
                /* Display header info */
                bmp_api_display_header_info((const struct s_bmp_header const *) &bmp_hdr_src);
                switch(parsed_cmdline_args.operation)
                {
                    case BMP_OP_FLIP:
                        switch(parsed_cmdline_args.suboperation)
                        {
                            case BMP_SUBOP_FLIP_LEFT_DIAGONAL:
                                /* Code to flip along Left diagonal */

                                break;
                            case BMP_SUBOP_FLIP_RIGHT_DIAGONAL:
                                /* Code to flip along Right diagonal */
                                bmp_api_flip_along_right_diagonal(fs,(const struct s_bmp_header const *) &bmp_hdr_src);
                                break;                            
                            case BMP_SUBOP_FLIP_MAX:
                            default:
                                /** 
                                 * Control shouldn't reach here,if it does, It means something seriously 
                                 * went wrong while populating parsed_cmdline_args structure in 
                                 * bmp_api_validate_cmdline_args() function
                                **/
                                printf( BOLDRED "Error:suboperation field not populated for FLIP operation" RESET );
                                break;
                        }
                        break;
                    case BMP_OP_CONVERT:
                        switch(parsed_cmdline_args.suboperation)
                        {
                            case BMP_SUBOP_CONVERT_MAXIMUM:
                                /* Code to convert with maximum algo */
                            case BMP_SUBOP_CONVERT_MINIMUM:
                                /* Code to convert with minimum algo */                            
                            case BMP_SUBOP_CONVERT_AVERAGE:
                                /* Code to convert with average algo */
                                bmp_api_convert(fs,(const struct s_bmp_header const *) &bmp_hdr_src);
                                break;
                            case BMP_SUBOP_CONVERT_MAX:
                            default:
                                /** 
                                 * Control shouldn't reach here,if it does, It means something seriously 
                                 * went wrong while populating parsed_cmdline_args structure in 
                                 * bmp_api_validate_cmdline_args() function
                                **/
                                printf( BOLDRED "Error:suboperation field not populated for convert operation" RESET );
                                break;
                        }
                        break;
                    case BMP_OP_FILTER:
                        switch(parsed_cmdline_args.suboperation)
                        {
                            case BMP_SUBOP_FILTER_MAXIMUM:
                                /* Code to apply filter with maximum algo */
                            case BMP_SUBOP_FILTER_MINIMUM:
                                /* Code to apply filter with minimum algo */
                            case BMP_SUBOP_FILTER_AVERAGE:
                                /* Code to apply filter with average algo */
                            case BMP_SUBOP_FILTER_MEDIAN:
                                /* Code to apply filter with median algo */
                                bmp_api_filter(fs,(const struct s_bmp_header const *) &bmp_hdr_src);
                                break;
                            case BMP_SUBOP_FILTER_MAX:
                            default:
                                /** 
                                 * Control shouldn't reach here,if it does, It means something seriously 
                                 * went wrong while populating parsed_cmdline_args structure in 
                                 * bmp_api_validate_cmdline_args() function
                                **/
                                printf( BOLDRED "Error:suboperation field not populated for filter operation" RESET );
                                break;
                        }
                        break;
                    case BMP_OP_MAX:
                        /** 
                         * Nothing to do here, This is when user merely wants to read header
                         * of BMP file which we have already displayed on screen 
                        **/
                       break;
                    default:
                    /** 
                     * Control shouldn't reach here,if it does, It means something seriously 
                     * went wrong while populating parsed_cmdline_args structure in 
                     * bmp_api_validate_cmdline_args() function
                    **/
                    printf( BOLDRED "Error:operation field not populated" RESET );                    
                    break;
                }
            }
        }

    }
    close(fs);
    return 0;    
}
