Usage is:
./bmp_api.0 <ImageName> [<operation> [<suboperation>]]
If an operation is provided then it's mandatory to provide suboperation.
List of valid <operation>    <suboperations>
                 flip            ld
                 flip            rd
-------------------------------------------
                convert          max
                convert          min
                convert          avg
-------------------------------------------
                filter           max
                filter           min
                filter           avg
                filter          median
Legend:
1.flip=flip the image
	ld=left diagonal[Support not given yet]
	rd=right diagonal
Example to flip "lenna.bmp" image file along right diagonal.
./bmp_api.io lenna.bmp flip rd


2.convert=convert to grayscale
	max=maximum of RGB
	min=minimum of RGB
	avg=average of RGB
Example to convert "saltpepper.bmp" image file to grayscale with average algorithm.
./bmp_api.io saltpepper.bmp convert max


3.filter=spatial filtering of grayscale image with 3x3 window
	max=maximum of all pixels in 3x3 window
	min=minimum of all pixels in 3x3 window
	avg=average of all pixels in 3x3 window
	median=median of all pixels in 3x3 window

Example to apply spatial filtering on "akshit.bmp" with median algo
./bmp_api.io akshit.bmp filter median

Header of the source image is displayed everytime any operation is performed.
If user still wants to read the header of say maurya.bmp file,use
./bmp_api.io maurya.bmp filter median
The utility is coded in such a way so as to provide support for rectangular images as well.
This code has been compiled and tested on Ubuntu 18.04 using gcc compiler.
For any more queries please contact
Akshit Maurya
gurdeepmaurya@gmail.com
8146744122,8588897406
