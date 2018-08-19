

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
    "\n               histogram       dump-true"
    "\n               histogram       dump-fuzzy"
    "\n--------------------------------------------\n"
    RESET);

}





/* This function Validates user arguments */
bool bmp_api_validate_cmdline_args(int argc, char * argv[])
{
    bool args_valid=false;
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
    else if((argc==4)&&(0==strcmp(argv[BMP_ARG_OPERATION],bmp_api_operation_str[BMP_OP_HISTOGRAM])))
    {
        /* User wants to dump true histogram of the pixelof image */
        if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_histogram_suboperation_str[BMP_SUBOP_HISTOGRAM_DUMP_TRUE]))
        {
            /* User wants to flip the Image along left diagonal */
            parsed_cmdline_args.operation=BMP_OP_HISTOGRAM;
            parsed_cmdline_args.suboperation=BMP_SUBOP_HISTOGRAM_DUMP_TRUE;
            args_valid=true;
        }
        else if(0==strcmp(argv[BMP_ARG_SUBOPERATION],bmp_api_histogram_suboperation_str[BMP_SUBOP_HISTOGRAM_DUMP_FUZZY])) 
        {
            /* User wants to flip the Image along right diagonal */
            parsed_cmdline_args.operation=BMP_OP_HISTOGRAM;
            parsed_cmdline_args.suboperation=BMP_SUBOP_HISTOGRAM_DUMP_FUZZY;
            args_valid=true;
        }
        else
        {
        printf(BOLDMAGENTA"\nInvalid suboperation \' %s\' for operation \'%s\'\n"RESET,argv[BMP_ARG_SUBOPERATION],argv[BMP_ARG_OPERATION]);
        }
    }/* Validation check for histogram operation ends here  */
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
        if((parsed_cmdline_args.operation==BMP_OP_HISTOGRAM)&&(BI_RGB!=GET_COMPRESSION(bmp_hdr_src_ptr)))
        {
            printf(BOLDMAGENTA "\nError:Input file should have compression type 'none'(0) for flip operation" RESET);
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

sprintf(outfilename,"%s_%d-%d-%d_%d-%d-%d.csv",custom_string, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}




void bmp_histogram_fuzzy(uint32_t *pixel_frequency)
{
    uint32_t * f_pixel_frequency=(uint32_t *) malloc((MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    int64_t i=0,h_i=0;
    uint64_t temp_frequency;
    if(NULL==f_pixel_frequency)
        {
        printf(BOLDRED"\nError:Can't allocate fuzzy pixel frequency buffer\n"RESET);
        }
    else
    {
        memset((void *) f_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {
            temp_frequency=0;
            for(h_i=0;h_i<HISTOGRAM_FUZZY_WINDOW;h_i++)
            {
                if((i+histogram_fuzzy_index[h_i])<0)
                {

                }
                else
                {
                    temp_frequency=temp_frequency+(pixel_frequency[i+histogram_fuzzy_index[h_i]]*histogram_fuzzy_weights[h_i]);

                }

            }
            temp_frequency/=14;
            f_pixel_frequency[i]=(uint32_t) temp_frequency;
        }
    
    memcpy((void *) pixel_frequency,(void *) f_pixel_frequency,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    free(f_pixel_frequency);
    }
}




/**
 * This function performs the actual dumping of histogram
**/


void bmp_api_histogram_dump(int fs,const struct s_bmp_header const * bmp_hdr_src_ptr)
{
FILE * fd_ptr=NULL;/* Destination file descriptor */
char outfilename[70]={0}; /* Destination file name */


/* Variables to keep track of pixel position */
uint32_t x=0,y=0;


uint32_t s_padding= GET_PADDING(bmp_hdr_src_ptr),
         s_offset=0,
         *gray_pixel_frequency=NULL,
         *red_pixel_frequency=NULL,
         *green_pixel_frequency=NULL,
         *blue_pixel_frequency=NULL;




ssize_t pixel_data=GET_RAW_BMP_SIZE(bmp_hdr_src_ptr),
        pixel_size=GET_PIXEL_SIZE(bmp_hdr_src_ptr),
        data_read=0;
ssize_t chunk_size=(pixel_data<CHUNK_SIZE)?pixel_data:CHUNK_SIZE;
int chunk_read=0;
uint8_t * s_buffer= (uint8_t *) malloc(pixel_data);

if(pixel_size == GRAY_SCALE_PIXEL_SIZE)
{
    gray_pixel_frequency=(uint32_t *) malloc((MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
}
else
{
    red_pixel_frequency  =(uint32_t *) malloc((MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    green_pixel_frequency=(uint32_t *) malloc((MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    blue_pixel_frequency =(uint32_t *) malloc((MAX_PIXEL_VALUE+1)*sizeof(uint32_t));        
}

if(parsed_cmdline_args.suboperation==BMP_SUBOP_HISTOGRAM_DUMP_TRUE)
{
    bmp_compose_output_file_name(outfilename,"T-HISTOGRAM");
    printf("\nIn true \n");
}
else if(parsed_cmdline_args.suboperation==BMP_SUBOP_HISTOGRAM_DUMP_FUZZY)
{
    bmp_compose_output_file_name(outfilename,"F-HISTOGRAM");
        printf("\nIn Fuzzy \n");
}
else
{
    bmp_compose_output_file_name(outfilename,"U-HISTOGRAM");
        printf("\nIn Unknown \n");
}

outfilename[69]='\0'; /* :P */

if(NULL==s_buffer)
{
    printf(BOLDRED"\nError:Can't allocate source buffer\n"RESET);
}
else if((pixel_size!=GRAY_SCALE_PIXEL_SIZE)&&((red_pixel_frequency==NULL)||(blue_pixel_frequency==NULL)||(green_pixel_frequency==NULL)))
{
        printf(BOLDRED"\nError:Can't allocate RGB buffer\n"RESET);
}
else if((pixel_size==GRAY_SCALE_PIXEL_SIZE)&&(gray_pixel_frequency==NULL))
{
    printf(BOLDRED"\nError:Can't allocate GRAY scale buffer\n"RESET);
}
else
{
    memset((void *) s_buffer,0,pixel_data);
    if(pixel_size == GRAY_SCALE_PIXEL_SIZE)
    {
        memset((void *) gray_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    }
    else
    {      
        memset((void *) red_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
        memset((void *) blue_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
        memset((void *) green_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(uint32_t));
    }

    /* Rewind the offset to beginning of BMP data in source file */
    if(GET_BMP_DATAOFFSET(bmp_hdr_src_ptr)!=lseek(fs,GET_BMP_DATAOFFSET(bmp_hdr_src_ptr),SEEK_SET))
    {
        printf(BOLDRED"\nError:Can't set cursor in source file"RESET);
    }
    else
    {
        /* Copy entire pixel data in source buffer */
        while(data_read!=pixel_data)
        {
            chunk_read=read(fs,(s_buffer+data_read),chunk_size);
            if(chunk_read==-1)
            {
                /* Facing trouble in reading source file */
                break;
            }
            else
            {
                data_read+=chunk_read;

                if((pixel_data-data_read)<chunk_size)
                    {
                    chunk_size=pixel_data-data_read;
                    }
            }

        }
        if(pixel_data!=data_read)
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
                    
                    /* Get value of this x,y pixel */
                    if(pixel_size == GRAY_SCALE_PIXEL_SIZE)
                    {
                     gray_pixel_frequency[(*(s_buffer+s_offset))]++;
                    }
                    else
                    {
                        blue_pixel_frequency[((struct s_RGB24 *)(s_buffer+s_offset))->blue]++;
                        green_pixel_frequency[((struct s_RGB24 *)(s_buffer+s_offset))->green]++;
                        red_pixel_frequency[((struct s_RGB24 *)(s_buffer+s_offset))->red]++;
                    }

                }/* Loop for Column pixel-Y scan */
            }/* Loop for row pixel-X scan */

            /* Do we need to Fuzz it up? */
            if(parsed_cmdline_args.suboperation == BMP_SUBOP_HISTOGRAM_DUMP_FUZZY)
            {
                if(pixel_size==GRAY_SCALE_PIXEL_SIZE)
                {
                  bmp_histogram_fuzzy(gray_pixel_frequency) ; 
                }
                else
                {
                  bmp_histogram_fuzzy(red_pixel_frequency) ;
                  bmp_histogram_fuzzy(green_pixel_frequency) ;
                  bmp_histogram_fuzzy(blue_pixel_frequency) ;
                }
            }            
            fd_ptr=fopen(outfilename,"w");
            if(fd_ptr==NULL)
            {
                printf(BOLDRED"\nError in creating destination file"RESET);
            }
            else
            {
                if(pixel_size==GRAY_SCALE_PIXEL_SIZE)            
                {
    
                    /* reusing s_offset here for loop index */
                    for(s_offset=0;s_offset<MAX_PIXEL_VALUE+1;s_offset++)
                        {
                        fprintf(fd_ptr,"%d,%d\n",s_offset,gray_pixel_frequency[s_offset]);
                        }

                }
                else
                {
                    /* reusing s_offset here for loop index */
                    for(s_offset=0;s_offset<MAX_PIXEL_VALUE+1;s_offset++)
                        {
                        fprintf(fd_ptr,"%d,%d,%d,%d\n",s_offset,red_pixel_frequency[s_offset],green_pixel_frequency[s_offset],blue_pixel_frequency[s_offset]);                                                
                        }

                }

            printf(BOLDGREEN"\nOutput file %s generated\n"RESET,outfilename);

            }
                
            fclose(fd_ptr);
        

        }
        
    }

    free(s_buffer);
    if(pixel_size==GRAY_SCALE_PIXEL_SIZE)
    {
        free(gray_pixel_frequency);
    }
    else
    {
        free(red_pixel_frequency);
        free(green_pixel_frequency);
        free(blue_pixel_frequency);
    }
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
                    case BMP_OP_HISTOGRAM:
                        switch(parsed_cmdline_args.suboperation)
                        {
                            case BMP_SUBOP_HISTOGRAM_DUMP_TRUE:
                                /* Code to dump true histogram */
                            case BMP_SUBOP_HISTOGRAM_DUMP_FUZZY:
                                bmp_api_histogram_dump(fs,(const struct s_bmp_header const *)&bmp_hdr_src);
                                break;                            
                            case BMP_SUBOP_HISTOGRAM_MAX:
                            default:
                                /** 
                                 * Control shouldn't reach here,if it does, It means something seriously 
                                 * went wrong while populating parsed_cmdline_args structure in 
                                 * bmp_api_validate_cmdline_args() function
                                **/
                                printf( BOLDRED "Error:suboperation field not populated for histogram operation" RESET );
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
