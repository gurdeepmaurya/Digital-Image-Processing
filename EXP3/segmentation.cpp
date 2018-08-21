#include <opencv2/opencv.hpp> //Include file for every supported OpenCV function
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include<stdint.h>
#include<stdlib.h>
#include<math.h>

#include"segmentation.hpp"

using namespace cv;
using namespace std;

/**
 * Validates commandline Arguments
**/
bool validate_cmdline_arguments(int argc,char *argv[],uint8_t &algo,bool &patch,bool &grow)
{
bool valid=false;

if(argc!=3)
{
    valid=false;
    printf("\n argc");
}

else if(0==strcmp(argv[2],"my"))
{
    algo=MY_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"entropy"))
{
    algo=ENTROPY_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"otsu"))
{
    algo=OTSU_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"my-fuzzy"))
{
    algo=MY_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"entropy-fuzzy"))
{
    algo=ENTROPY_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"otsu-fuzzy"))
{
    algo=OTSU_ALGO;
    valid=true;
}
else if(0==strcmp(argv[2],"my-patch"))
{
    algo=MY_ALGO;
    patch=true;
    valid=true;
}
else if(0==strcmp(argv[2],"entropy-patch"))
{
    algo=ENTROPY_ALGO;
    patch=true;
    valid=true;
}
else if(0==strcmp(argv[2],"otsu-patch"))
{
    algo=OTSU_ALGO;
    valid=true;
    patch=true;
}
else if(0==strcmp(argv[2],"my-fuzzy-patch"))
{
    algo=MY_ALGO;
    valid=true;
    patch=true;
}
else if(0==strcmp(argv[2],"entropy-fuzzy-patch"))
{
    algo=ENTROPY_ALGO;
    valid=true;
    patch=true;
}
else if(0==strcmp(argv[2],"otsu-fuzzy-patch"))
{
    algo=OTSU_ALGO;
    valid=true;
    patch=true;
}
else if(0==strcmp(argv[2],"grow"))
{
    valid=true;
    grow=true;
}
else 
{
    valid = false;
    printf("\nHere");
}

return valid;
} 

/**
 * This Function Gathers Histogram Data
**/

void gather_histogram_data(Mat &img,long double * pdf)
{
    uint8_t *pixel_ptr=(uint8_t *) img.data;
    int64_t i=0,
            j=0,
            channels=img.channels(),
            rows=img.rows,
            cols=img.cols;

    /* Gather histogram */

        for(i=0;i<rows;i++)
        {
            for(j=0;j<cols;j++)
            {
                pdf[(pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)])]++;
            }

        }

}

/*******************Overloaded Variant*************/



void gather_histogram_data(Mat &img,long double * pdf,uint16_t lrow,uint16_t urow,uint16_t lcol,uint16_t ucol)
{
    uint8_t *pixel_ptr=(uint8_t *) img.data;;
    int64_t i=0,
            j=0,
            channels=img.channels(),
            cols=img.cols;

    /* Gather histogram */

        for(i=lrow;i<urow;i++)
        {
            for(j=lcol;j<ucol;j++)
            {
                pdf[(pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)])]++;
            }

        }
    cout<<"\n Histogram Gathered for row="<<lrow<<"-"<<urow<<"and column="<<lcol<<"-"<<ucol<<endl;

}

/**
 * This function writes a histogram into CSV file
**/

void writeHistogram(const char * custom_str,long double *pdf)
{
    uint64_t i=0;
    char histFile[100]={0};
    
    /* Dump Histogram of Input file */
    FILE * fd_ptr;
    strcpy(histFile,custom_str);
    strcat(histFile,"histogram");
    strcat(histFile,".csv");
    histFile[99]=0;

    fd_ptr=fopen(histFile,"w");
    if(fd_ptr==NULL)
        {
            printf("\n[Error] in creating destination file");
        }
    else
        {   
            for(i=0;i<MAX_PIXEL_VALUE+1;i++)
                {
                fprintf(fd_ptr,"%ld,%ld\n",i,(int64_t )pdf[i]);
                }
            printf("\n[SUCCESS]Input Image Histogram file %s generated\n",histFile);
            fclose(fd_ptr);
        }
}



/**
 * Fuzzy Histogram function
**/

void fuzz_it_up(long double *pixel_frequency)
{
    long double * f_pixel_frequency=(long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
    int64_t i=0,h_i=0;
    long double temp_frequency;
    if(NULL==f_pixel_frequency)
        {
        printf("\nError:Can't allocate fuzzy pixel frequency buffer\n");
        }
    else
    {
        memset((void *) f_pixel_frequency,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {
            temp_frequency=0;
            for(h_i=0;h_i<HISTOGRAM_FUZZY_WINDOW;h_i++)
            {
                if((i+histogram_fuzzy_index[h_i])<0||(i+histogram_fuzzy_index[h_i])>MAX_PIXEL_VALUE)
                {

                }
                else
                {
                    temp_frequency=temp_frequency+(pixel_frequency[i+histogram_fuzzy_index[h_i]]*(long double)histogram_fuzzy_weight[h_i]);
                }

            }
            temp_frequency/=14;
            f_pixel_frequency[i]= temp_frequency;
        }
    
    memcpy((void *) pixel_frequency,(void *) f_pixel_frequency,(MAX_PIXEL_VALUE+1)*sizeof(long double));
    free(f_pixel_frequency);
    }
}

/**
 * This Function segments the Image
**/
void segment_img(Mat &img, uint8_t threshold)
{
    uint8_t *pixel_ptr=(uint8_t *) img.data;;
    int64_t i=0,
            j=0,
            channels=img.channels(),
            rows=img.rows,
            cols=img.cols;

    for(i=0;i<rows;i++)
    {
        for(j=0;j<cols;j++)
        {
            if(pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)] < threshold)
            {
                pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)]=0;
            }
            else
            {
                pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)]=MAX_PIXEL_VALUE;
            }                
        }

    }
}

void segment_img(Mat &img, uint8_t threshold,uint16_t lrow,uint16_t urow,uint16_t lcol,uint16_t ucol)
{
    uint8_t *pixel_ptr=(uint8_t *) img.data;;
    int64_t i=0,
            j=0,
            channels=img.channels(),
            cols=img.cols;

    for(i=lrow;i<urow;i++)
    {
        for(j=lcol;j<ucol;j++)
        {
            if(pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)] < threshold)
            {
                pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)]=0;
            }
            else
            {
                pixel_ptr[PIXEL_OFFSET(i,j,cols,channels)]=MAX_PIXEL_VALUE;
            }                
        }

    }
}






/**
 * This function computes mean of series including lower bound but not upper bound
**/
long double compute_mean(uint16_t lower_bound,uint16_t upper_bound,long double *pdf)
{
    int i=0;
    long double inner_product=0;
    uint64_t total_pixel=0;
    for(i=lower_bound;i<upper_bound;i++)
    {
        total_pixel+=(uint64_t)pdf[i];
        inner_product+=(i*pdf[i]);
    }
    return (inner_product/total_pixel);
}


/**
 * This function computes threshold within MAX_THRESHOLD_ERROR range
**/ 

uint8_t compute_iterative_threshold(long double *pdf)
{
    long double threshold_error=MAX_PIXEL_VALUE,
                mean1=0,
                mean2=0,
                threshold=MAX_PIXEL_VALUE/2;
    int iteration=0;
    while(false==isless(threshold_error,(long double)MAX_THRESHOLD_ERROR))
    {
        printf("\n\nIteration %d"
                "\nThreshold=%Lf"
                ,iteration
                ,threshold);
               
        mean1 = compute_mean(0,threshold,pdf);
        mean2 = compute_mean(threshold,MAX_PIXEL_VALUE+1,pdf);
        threshold_error = (((mean1+mean2)/2)-threshold);        
        threshold=(mean1+mean2)/2;
        printf("\nmean1=%Lf"
               "\nmean2=%Lf"
               "\nthreshold_error=%Lf"
               "\nNew threshold=%Lf",
               mean1,
               mean2,
               threshold_error,
               threshold); 
        if(true==isless(threshold_error,(long double)0))
        {
            threshold_error*=(-1);
        }
        iteration++;
    }
    printf("\n\n"
            "\n----------------------------------"
            "\nIterations to calculate mean=%d"
            "\nThreshold= %Lf"
            "\n---------------------------------"
            ,iteration
            ,threshold);
    return ((uint8_t)threshold);
}




void my_algo(Mat &img,long double * pdf)
{
    uint8_t threshold=0;

    threshold=compute_iterative_threshold(pdf);
    printf("\n\n\nActual Iterative Threshold=%u",threshold);

    segment_img(img,threshold);
}

void my_algo(Mat &img,long double * pdf,uint16_t lrow,uint16_t urow,uint16_t lcol,uint16_t ucol)
{
    uint8_t threshold=0;

    threshold=compute_iterative_threshold(pdf);
    printf("\n\n\nActual Iterative Threshold=%u",threshold);

    segment_img(img,threshold,lrow,urow, lcol, ucol);
}

/**
 * 
 * Function which implements Otsu algo 
 * 
*/

uint8_t compute_otsu_threshold(long double *pdf,size_t total_pixel)
{
    long double class_mean1=0,
                class_mean2=0,
                class_probability1=0,
                class_probability2=0,
                prev_class_probability1=0,
                prev_class_probability2=0,
                between_class_variance=0,
                max_between_class_variance=0;

    uint16_t threshold=0,otsu_threshold=0;

    while(0==pdf[threshold])
    {
        threshold++;
    }

    class_mean1 = threshold; 
    class_probability1=pdf[threshold]/total_pixel;
    class_mean2 = compute_mean(threshold+1,MAX_PIXEL_VALUE+1,pdf);
    class_probability2=1-class_probability1;
    
    between_class_variance = class_probability1*class_probability2*((class_mean2-class_mean1)*(class_mean2-class_mean1));
    max_between_class_variance=between_class_variance;
    otsu_threshold=threshold;

    while(threshold++<MAX_PIXEL_VALUE)
    {
        prev_class_probability1=class_probability1;
        prev_class_probability2=class_probability2;
        class_probability1=class_probability1+(pdf[threshold]/total_pixel);           
        class_probability2=1-class_probability1;
        
        if(true==islessequal(1,class_probability1))
        {
            break;
        }

        class_mean1=((prev_class_probability1*class_mean1)+(threshold*(pdf[threshold]/total_pixel)))/class_probability1;
        class_mean2=((prev_class_probability2*class_mean2)-(threshold*(pdf[threshold]/total_pixel)))/class_probability2;

        between_class_variance = class_probability1*class_probability2*((class_mean2-class_mean1)*(class_mean2-class_mean1));

        printf("\n\n-------------------------------------------"
                "\nThreshold=%d"
                "\nBetween class variance=%Lf"
                "\nclass_mean1=%Lf"
                "\nclass_mean2=%Lf"
                "\nclass_probability1=%Lf"
                "\nclass_probability2=%Lf"
                ,                
                threshold,
                between_class_variance,
                class_mean1,
                class_mean2,
                class_probability1,
                class_probability2);
        if(true==isgreater(between_class_variance,max_between_class_variance))
        {
            max_between_class_variance=between_class_variance;
            otsu_threshold=threshold;
        }

    }

    return ((uint8_t)otsu_threshold);
}

void otsu_algo(Mat &img,long double * pdf)
{
    uint8_t threshold=0;

    threshold=compute_otsu_threshold(pdf, img.total());
    printf("\n\n\nActual Otsu Threshold=%u",threshold);

    segment_img(img,threshold);
}
/*---------Overloaded Variants-----------------------------------------*/

void otsu_algo(Mat &img,long double * pdf,uint16_t lrow,uint16_t urow,uint16_t lcol,uint16_t ucol)
{
    uint8_t threshold=0;

    threshold=compute_otsu_threshold(pdf, img.total());
    printf("\n\n\nActual Otsu Threshold=%u",threshold);

    segment_img(img,threshold,lrow,urow,lcol,ucol);
}

/**********************************************************************/

/**
 * This function implemets Entropy algo
**/
uint8_t compute_entropy_threshold(long double *pdf)
{
    long double entropy=0,max_entropy=0,temp=0;
    uint64_t total_pixel1=0,total_pixel2=0;

    uint16_t threshold=0,entropy_threshold=0,i=0;;

    for(threshold=0;threshold<=MAX_PIXEL_VALUE;threshold++)
    {
        entropy=0;
        total_pixel1=0;
        total_pixel2=0;


        for(i=0;i<threshold;i++)
        {
        total_pixel1+=pdf[i];
        }


        for(i=threshold;i<=MAX_PIXEL_VALUE;i++)
        {
            total_pixel2+=pdf[i];
        }

        for(i=0;i<threshold;i++)
        {
            if(0==total_pixel1)
            {
                continue;
            }


            if(0==pdf[i])
            {
                continue;
            }
            temp=(total_pixel1)/pdf[i];
            entropy+=((log2l(temp))/temp);
        }

        for(i=threshold;i<=MAX_PIXEL_VALUE;i++)
        {
            if(0==total_pixel2)
            {
                continue;
            }

            if(0==pdf[i])
            {
                continue;
            }
            temp=(total_pixel2)/pdf[i];
            entropy+=(log2l(temp)/temp);
        }
        cout<<"\nThreshold:"<<threshold<<" total_pixel1:"<<total_pixel1<<" total_pixel2:"<<total_pixel2<<" entropy:"<<entropy<<endl;
        if(true==isgreater(entropy,max_entropy))
        {
            max_entropy=entropy;
            entropy_threshold=threshold;
        }
    }
    return ((uint8_t)entropy_threshold);
}

void entropy_algo(Mat &img,long double * pdf,uint16_t lrow,uint16_t urow,uint16_t lcol,uint16_t ucol)
{
    uint8_t threshold=0;

    threshold=compute_entropy_threshold(pdf);
    printf("\n\n\nActual Entropy Threshold=%u",threshold);

    segment_img(img,threshold,lrow,urow,lcol,ucol);
}

void entropy_algo(Mat &img,long double * pdf)
{
    uint8_t threshold=0;

    threshold=compute_entropy_threshold(pdf);
    printf("\n\n\nActual Entropy Threshold=%u",threshold);

    segment_img(img,threshold);
}

















void mouseEvent(int evt, int x, int y, int flags, void* param) 
{                    
    uint64_t* offset = (uint64_t *) param;
    if (evt == CV_EVENT_LBUTTONDOWN) 
    { 
        if(offset[1]==0)
        {
            offset[1]= PIXEL_OFFSET(x,y,offset[0],1);
        }
        else
        {
            offset[2]= PIXEL_OFFSET(x,y,offset[0],1);
        } 
    }         
}
















int main( int argc, char* argv[] ) 
{

bool patch=false,grow=false;

uint8_t algo=0xFF;

char outputImg[100]={0};
strcpy(outputImg,"O_");
strcat(outputImg,argv[1]);
outputImg[99]=0;


Mat img = imread(argv[1],IMREAD_ANYCOLOR);

if (false == validate_cmdline_arguments(argc,argv,algo,patch,grow))
{
    printf("\nusage: ./segmentation <SourceFile>  <Algo>"
           "\nValid values of <Algo>: my,my-fuzzy,entropy,entropy-fuzzy,otsu,otsu-fuzzy");

}
else if(true==img.empty() ) 
{
    cout<<"\nError in Loading Image"<<endl;

}
else if(GRAY_IMG_CHANNEL!=img.channels())
{
    printf("\nThou shall only input grayscale image.");
}
else if((false==patch)&&(false==grow))
{
    cout<<"\nNumber of Channels:"<<img.channels()<<endl;
    namedWindow( "Input", WINDOW_AUTOSIZE );
    namedWindow( "Output", WINDOW_AUTOSIZE );
    imshow( "Input", img );

    long double * pdf=NULL;

    pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
    memset(pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

    gather_histogram_data(img,pdf);
        
    writeHistogram("I_",pdf);

    switch(algo)
    {
        case MY_ALGO:
                    my_algo(img,pdf);
                    break;
        case ENTROPY_ALGO:
                    entropy_algo(img,pdf);
                    break;
        case OTSU_ALGO:
                    otsu_algo(img,pdf);
                    break;
        case FUZZY_MY_ALGO:
                    fuzz_it_up(pdf);
                    my_algo(img,pdf);
                    break;
        case FUZZY_ENTROPY_ALGO:
                    fuzz_it_up(pdf);
                    entropy_algo(img,pdf);
                    break;
        case FUZZY_OTSU_ALGO:
                    fuzz_it_up(pdf);
                    otsu_algo(img,pdf);
                    break;
        default:
        printf("\n[ERROR]:unexpected algo value");
    }

    //writeHistogram("FI_",pdf);

    free(pdf);
    imshow( "Output", img );
    waitKey( 0 );
    destroyWindow( "Input" );
    destroyWindow( "Output" );

    imwrite( outputImg, img );

    printf("\n"); //'cause I have OCD

}/* Gray Scale code ends here*/
else if(true==patch)
{
    cout<<"Inside Patch"<<endl;
    cout<<"\nNumber of Channels:"<<img.channels()<<endl;
    namedWindow( "Input", WINDOW_AUTOSIZE );
    namedWindow( "Output", WINDOW_AUTOSIZE );
    imshow( "Input", img );

    long double * pdf=NULL;

    uint64_t i=0,j=0,row_step_size=0,col_step_size=0;

    row_step_size=img.rows/4;


    pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));

    for(i=0;i<(uint64_t)img.rows;i+=row_step_size)
    {
    col_step_size=img.cols/4;

        for(j=0;j<(uint64_t)img.cols;j+=col_step_size)
        {

            memset(pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

            gather_histogram_data(img,pdf,i,i+row_step_size,j,j+col_step_size);
        
            //writeHistogram("I_",pdf);

            switch(algo)
            {
                case MY_ALGO:
                            my_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                case ENTROPY_ALGO:
                            entropy_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                case OTSU_ALGO:
                            otsu_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                case FUZZY_MY_ALGO:
                            fuzz_it_up(pdf);
                            my_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                case FUZZY_ENTROPY_ALGO:
                            fuzz_it_up(pdf);
                            entropy_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                case FUZZY_OTSU_ALGO:
                            fuzz_it_up(pdf);
                            otsu_algo(img,pdf,i,i+row_step_size,j,j+col_step_size);
                            break;
                default:
                printf("\n[ERROR]:unexpected algo value");
            }

        col_step_size=((img.cols-j)<col_step_size)?(img.cols -j):col_step_size;
        }
    row_step_size=((img.rows-i)<row_step_size)?(img.rows -i):row_step_size;
    }

    //writeHistogram("FI_",pdf);

    free(pdf);
    imshow( "Output", img );
    waitKey( 0 );
    destroyWindow( "Input" );
    destroyWindow( "Output" );

    imwrite( outputImg, img );

    printf("\n"); //'cause I have OCD

}
else if(true==grow)
{
    uint8_t *pixel_ptr=(uint8_t *) img.data;
    uint64_t *pixel_stack=(uint64_t *) malloc(img.rows*img.cols*sizeof(uint64_t));

    memset(pixel_stack,0,img.rows*img.cols*sizeof(uint64_t));

    cout<<"Inside Patch"<<endl;
    cout<<"\nNumber of Channels:"<<img.channels()<<endl;
    namedWindow( "Input", WINDOW_AUTOSIZE );
    //namedWindow( "Output", WINDOW_AUTOSIZE );

    uint64_t offset[3]={0};
    int seed_pixel=0;
    offset[0]=img.cols;
    setMouseCallback("Input", mouseEvent, &offset);
    imshow( "Input", img );
    waitKey( 0 );

    if(0!=offset[1])
    {
        cout<<"offset[1] is:"<<offset[1]<<endl;


    }
    if(0!=offset[2])
    {
        cout<<"offset[2] is:"<<offset[2]<<endl;
    }


}
else
{
    printf("\nI shouldn't be Here,Something's Wrong");
}   


return 0;
}


/**
 * The following routines implement stack for pushing and popping
 * Neighbouring pixel offset
**/
uint64_t * stack_bottom=NULL,*stack_top=NULL,stack_size=0;
void init_stack(uint64_t * base_ptr,uint64_t set_stack_size)
{
    stack_bottom=base_ptr;
    stack_bottom--;
    stack_top=stack_bottom;
    stack_size=set_stack_size;

}

bool push_pixel_offset(uint64_t offset)
{
    bool ret_val=false;
    if(stack_size==(stack_top-stack_bottom))
    {
        printf("\n--------------------------"
               "\n[ERROR]:Stack Overflow");
    }
    else
    {
        stack_top++;
        *stack_top=offset;
        ret_val=true;
    }

    return(ret_val);
}

bool pop_pixel_offset(uint64_t &offset)
{
    bool ret_val=false;
    if(stack_top==stack_bottom)
    {
        printf("\n--------------------------"
               "\n[ERROR]:Stack Underflow");
    }
    else
    {
        offset=*stack_top;
        stack_top--;
        ret_val=true;
    }

    return(ret_val);
}

bool pixel_offset_found(uint64_t offset)
{
    bool ret_val=false;
    uint64_t *temp_stack_ptr=stack_bottom;
    while(stack_top!=temp_stack_ptr++)
    {
        if(offset==*temp_stack_ptr)
        {
        ret_val=true;
        break;
        }

    }

    return(ret_val);
}
