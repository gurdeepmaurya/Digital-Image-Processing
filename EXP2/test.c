#define TESTING_STUFF

#ifdef TESTING_STUFF

#include<stdio.h>
#include"bmp_api.h"
#include<stdlib.h>
#include<string.h>
void bmp_bubble_sort(uint8_t * a)
{
    uint8_t i,j,temp;
for(i=1;i<F_WIDTH*F_HEIGHT;i++)
{
    for(j=0;j<(F_WIDTH*F_HEIGHT-i);j++)
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

int main(int argc,char * argv[])
{
    uint8_t a[10]={8,8,7,5,4,5,10,2,1,2};
    uint32_t x=0,y=0;
    int8_t f_x=-1,f_y=-1;
/* Ascending order bubble sort 
bmp_bubble_sort(a);
printf("\n");

for(i=0;i<F_WIDTH*F_HEIGHT;i++)
printf("%d ",a[i]);

printf("\nMedian:%d",(a[10/2]+a[(10/2)-1])/2);
*/
 if((((int32_t)x+f_x)<0)||(((int32_t)y+f_y)<0))
 printf("here");
    return 0;
}



#endif