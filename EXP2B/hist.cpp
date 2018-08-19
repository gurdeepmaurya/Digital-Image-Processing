#include <opencv2/opencv.hpp> //Include file for every supported OpenCV function
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include<stdint.h>
#include<stdlib.h>
#include<math.h>

#include"hist.hpp"

using namespace cv;
using namespace std;


int main( int argc, char* argv[] ) 
{
uint8_t *pixel_ptr=NULL;
uint32_t i=0,j=0,channels=0;
long double gamma =0;
char histFile[100]={0};


if(argc!=5)
{
    printf("\nusage: ./hist <SourceFile> <OutputFile> <HistogramFileNameWithoutExtension> <equilize/gamma>\n");
    return -1;
}
if(0==strcmp(argv[4],"gamma"))
{
    printf("\nEnter Value of gamma:");
    scanf("%Lf",&gamma);
}
Mat img = imread(argv[1],IMREAD_ANYCOLOR);

if( img.empty() ) 
{
    cout<<"Error in Loading Image"<<endl;
    return -1;
}
else
{
    namedWindow( "Input", WINDOW_AUTOSIZE );
    namedWindow( "Output", WINDOW_AUTOSIZE );
    imshow( "Input", img );

    pixel_ptr=(uint8_t *) img.data;
    channels=img.channels();
    if(GRAY_IMG_CHANNEL==channels)
    {
        long double * pdf=NULL;

        pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));



        cout<<"\nNumber of Channels:"<<channels<<endl;

        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                pdf[(pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)])]++;
                //printf("\n%u   %u   %u",i+1,j+1,pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)]);
            }

        }
        
        /* Dump Histogram of Input file */
        FILE * fd_ptr;
        strcpy(histFile,"I_");
        strcat(histFile,argv[3]);
        strcat(histFile,".csv"); 
        fd_ptr=fopen(histFile,"w");
        if(fd_ptr==NULL)
        {
            printf("\n[Error] in creating destination file");
            return -1;
        }
        else
        {   
        /* reusing i here for loop index */
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
            {
            fprintf(fd_ptr,"%d,%d\n",i,(uint8_t )pdf[i]);
            }
        printf("\n[SUCCESS]Input Image Histogram file %s generated\n",histFile);
        fclose(fd_ptr);
        }

         
                


        /* Normalize the PDF */
        j=img.total();
        printf("\n\n\n"
               "-----------Computing PDF now---------------");    
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {        
            pdf[i]/=j;
            printf("\n%d           %Lf",i,pdf[i]);
        }

        /* Choose between gamma or Equilization*/
        if(0==strcmp(argv[4],"gamma"))
        {
            printf("\n\n\n"
                   "-----------Applying Gamma now---------------");    
            /* Compute the CDF */
            for(i=1;i<MAX_PIXEL_VALUE+1;i++)
            {        
                pdf[i]=powl(pdf[i],gamma);
                printf("\n%d           %Lf",i,pdf[i]);
            }

        }
        else
        {
            printf("\n\n\n"
                   "-----------Computing CDF now---------------");    
            /* Compute the CDF */
            for(i=1;i<MAX_PIXEL_VALUE+1;i++)
            {        
                pdf[i]+=pdf[i-1];
                printf("\n%d           %Lf",i,pdf[i]);
            }

        }

        printf("\n\n\n"
               "-----------Computing Normalized pixel now---------------");    
        /* Compute the Normalized pixel value */
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {        
            pdf[i]*=MAX_PIXEL_VALUE;
            pdf[i]=((true==isless(255.0,pdf[i]))?255:pdf[i]);
            printf("\n%d           %u",i,(uint8_t)pdf[i]);
        }

        /* Perform Transformation */
        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)]=(uint8_t) pdf[(pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)])];

            }

        }


    /* Output Histogram CSV file */
        strcpy(histFile,"O_");    
        strcat(histFile,argv[3]);
        strcat(histFile,".csv"); 
        fd_ptr=fopen(histFile,"w");
        if(fd_ptr==NULL)
        {
            printf("\n[Error] in creating destination file");
            return -1;
        }
        else
        {   
        /* reusing i here for loop index */
        memset(pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                pdf[(pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)])]++;
                //printf("\n%u   %u   %u",i+1,j+1,pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)]);
            }

        }


        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
            {
            fprintf(fd_ptr,"%d,%d\n",i,(uint8_t )pdf[i]);
            }
        printf("\n[SUCCESS]Output Image Histogram file %s generated\n",histFile);
        fclose(fd_ptr);
        }

    free(pdf);
    }/* Gray Scale code ends here*/
    else
    {
        /* Image is RGB */
        long double *r_pdf=NULL,
               *g_pdf=NULL,
               *b_pdf=NULL;
        
        struct s_BGR24 bgr={0};

        r_pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(r_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

        g_pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(g_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

        b_pdf= (long double *) malloc((MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(b_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));


        cout<<"\nNumber of Channels:"<<channels<<endl;

        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                bgr = *((struct s_BGR24 *) (pixel_ptr + PIXEL_OFFSET(i,j,img.cols,channels)));
                b_pdf[bgr.blue]++;
                g_pdf[bgr.green]++;
                r_pdf[bgr.red]++;                                
            }

        }

        /* Dump Histogram of Input file */
        FILE * fd_ptr;
        strcpy(histFile,"I_");
        strcat(histFile,argv[3]);
        strcat(histFile,".csv"); 
        fd_ptr=fopen(histFile,"w");
        if(fd_ptr==NULL)
        {
            printf("\n[Error] in creating destination file");
            return -1;
        }
        else
        {   
        /* reusing i here for loop index */
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
            {
            fprintf(fd_ptr,"%d,%d,%d,%d\n",i,(uint8_t )r_pdf[i],(uint8_t )g_pdf[i],(uint8_t )b_pdf[i]);
            }
        printf("\n[SUCCESS]Input Image Histogram file %s generated\n",histFile);
        fclose(fd_ptr);
        }






        /* Normalize the PDF */
        j=img.total();
        printf("\n\n\n"
               "-----------Computing PDF now---------------");    
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {        
            b_pdf[i]/=j;
            g_pdf[i]/=j;
            r_pdf[i]/=j;                        
            printf("\n%d           %Lf         %Lf         %Lf",i,b_pdf[i],g_pdf[i],r_pdf[i]);
        }

        /* Choose between gamma or Equilization*/
        if(0==strcmp(argv[4],"gamma"))
        {
            printf("\n\n\n"
                   "-----------Applying Gamma now---------------");    
            /* Apply Gamma */
            for(i=1;i<MAX_PIXEL_VALUE+1;i++)
            {        
                b_pdf[i]=powl(b_pdf[i],gamma);
                g_pdf[i]=powl(g_pdf[i],gamma);
                r_pdf[i]=powl(r_pdf[i],gamma);
                printf("\n%d           %Lf         %Lf         %Lf",i,b_pdf[i],g_pdf[i],r_pdf[i]);
            }

        }
        else
        {
            printf("\n\n\n"
                   "-----------Computing CDF now---------------");    
            /* Compute the CDF */
            for(i=1;i<MAX_PIXEL_VALUE+1;i++)
            {        
                b_pdf[i]+=b_pdf[i-1];
                g_pdf[i]+=g_pdf[i-1];
                r_pdf[i]+=r_pdf[i-1];
                printf("\n%d           %Lf         %Lf         %Lf",i,b_pdf[i],g_pdf[i],r_pdf[i]);
            }
        }

        printf("\n\n\n"
               "-----------Computing Normalized pixel now---------------");    
        /* Compute the Normalized pixel value */
        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
        {        
            b_pdf[i]*=MAX_PIXEL_VALUE;
            g_pdf[i]*=MAX_PIXEL_VALUE;
            r_pdf[i]*=MAX_PIXEL_VALUE;
            b_pdf[i]=((true==isless(255.0,b_pdf[i]))?255:b_pdf[i]);
            g_pdf[i]=((true==isless(255.0,g_pdf[i]))?255:g_pdf[i]);
            r_pdf[i]=((true==isless(255.0,r_pdf[i]))?255:r_pdf[i]);
            printf("\n%d           %u         %u         %u",i,(uint8_t)b_pdf[i],(uint8_t)g_pdf[i],(uint8_t)r_pdf[i]);
        }

        /* Perform Transformation */
        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                bgr= *((struct s_BGR24 *)(pixel_ptr + PIXEL_OFFSET(i,j,img.cols,channels)));
                
                bgr.blue = (uint8_t) b_pdf[bgr.blue];
                bgr.green = (uint8_t) g_pdf[bgr.green];
                bgr.red = (uint8_t) r_pdf[bgr.red];

                *((struct s_BGR24 *)(pixel_ptr + PIXEL_OFFSET(i,j,img.cols,channels))) = bgr;
                //printf("\n%u   %u   %u",i+1,j+1,pixel_ptr[PIXEL_OFFSET(i,j,img.cols,channels)]);
            }

        }


    /* Output Histogram CSV file */
        strcpy(histFile,"O_");    
        strcat(histFile,argv[3]);
        strcat(histFile,".csv"); 
        fd_ptr=fopen(histFile,"w");
        if(fd_ptr==NULL)
        {
            printf("\n[Error] in creating destination file");
            return -1;
        }
        else
        {   
        /* reusing i here for loop index */
        memset(r_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(g_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));
        memset(b_pdf,0,(MAX_PIXEL_VALUE+1)*sizeof(long double));

        for(i=0;i<img.rows;i++)
        {
            for(j=0;j<img.cols;j++)
            {
                bgr = *((struct s_BGR24 *) (pixel_ptr + PIXEL_OFFSET(i,j,img.cols,channels)));
                b_pdf[bgr.blue]++;
                g_pdf[bgr.green]++;
                r_pdf[bgr.red]++;                                
            }

        }

        for(i=0;i<MAX_PIXEL_VALUE+1;i++)
            {
            fprintf(fd_ptr,"%d,%d,%d,%d\n",i,(uint8_t )r_pdf[i],(uint8_t )g_pdf[i],(uint8_t )b_pdf[i]);
            }
        printf("\n[SUCCESS]Output Image Histogram file %s generated\n",histFile);
        fclose(fd_ptr);
        }



    free(b_pdf);
    free(g_pdf);
    free(r_pdf);
    }







}
imshow( "Output", img );
waitKey( 0 );
destroyWindow( "Input" );
destroyWindow( "Output" );
printf("\n"); //'cause I have OCD
imwrite( argv[2], img );
return 0;
}







/*


*/