///////////////////////////////////////////////////
// File name 	: main.cpp
// Description 	: This is the main program that calls
//		: all the functions in the face detection
//		: pipeline.
// Author 	: Saroj Bardewa
// Revision	: 03/12/2017 v.01
///////////////////////////////////////////////////
#include <iostream>
#include <stdlib.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "timer.h"


using namespace cv;
using namespace std; 

//////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////
void showImage(Mat &img, string windowName);
Mat k_rgb2Gray(Mat &inImage,Mat &outputGrayImage,float &runtime);
Mat k_normalize(Mat &inGrayImage,Mat &tempImage, float & runtime);

/*****************************************************************/
int main(int argc, char **argv) {
	// If no input image supplied, quit.
	float exTime;
	if(argc <2)
	{	
		cout <<"Error!!! Supply an input image <command><input image>" <<endl;
		exit(1);
	}
	
	// Read the input image
	Mat inImage;
	inImage = imread(argv[1],CV_LOAD_IMAGE_UNCHANGED); 
	
	// If the image is empty, quit.
	if(inImage.empty())
	{
		cout << "Empty image. Terminating ..." <<endl;
		exit(1);
	}
	
	// Create a temporary image which will be used as 
	// an image container in later functions
	Mat tempImage;
	tempImage.create(inImage.rows, inImage.cols, CV_8UC1);	
	
	// Call a function to convert input RGB image to Grayscale image
	// and display the computation time
	Mat grayImage;
        if(inImage.channels() > 1)  
	{
		// Image is not already a grayscale image, so convert it
		grayImage = k_rgb2Gray(inImage,tempImage,exTime);
		cout << "\n****************************************" <<endl;
		cout << "GPU_Grayscale Computation time : " <<exTime << " ms" <<endl;
	}
	else
	 	grayImage = inImage.clone();

	// Call the function to normalize the grayscale image	
	Mat normImage = k_normalize(grayImage,tempImage,exTime);
	cout << "GPU_Normalization Computation time : " <<exTime << " ms" <<endl;
	
	showImage(inImage,"ORIGINAL IMAGE");
	showImage(grayImage,"GRAYSCALE IMAGE");
	showImage(normImage, "NORMALIZED IMAGE");
	waitKey(0);

return 0;
}

/********************************************************************/
// Function Definitions
/********************************************************************/
//////////////////////////////////////////////////
// showImage : Creates a temporary window and 
//	     : displays the image
//////////////////////////////////////////////////
void showImage(Mat &img, string windowName)
{
	namedWindow(windowName);
	imshow(windowName,img);
}


