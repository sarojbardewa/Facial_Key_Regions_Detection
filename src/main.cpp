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

#include "globalVar.h"
#include "timer.h"
using namespace cv;
using namespace std; 
////////////////////////////
//Macros
///////////////////////////
// Only one of these should
// be defined at one time
//#define CPU_EX
#define GPU_EX
//#define OCV_EX
////////////////////////////
// One or both of these must
// be defined
#define CASCADE
#define DISPLAY
//////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////
void showImage(Mat &img, string windowName);

//CPU functions
void h_rgb2Gray(Mat &inRGBImage,Mat &tempImage,float &runtime);
void h_normalize(Mat &inGrayImage,Mat &tempImage,float &runtime);

// OpenCV routines
void ocv_rgb2Gray(Mat &inRGBImage, Mat &grayImage,float &runtime);
void ocv_normalize(Mat &inGrayImage, Mat &normImage,float &runtime);
int detectAndDisplay(Mat &inNormImage, Mat &originalImage);
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
	
	// Start clock to get total time
	//Start the CPU clock
	GpuTimer gpuClk;
	gpuClk.Start();

	
//**********************************************************************
	////////////////////////////////////////////////////////////
	// CPU Functions
	///////////////////////////////////////////////////////////
#ifdef CPU_EX
	//------------------------------------------------------------//
	// Convert RGB to Gray
	char message[] = "CPU";
	Mat grayImage = Mat::zeros(inImage.rows, inImage.cols, CV_8UC1);
	Mat normImage = Mat::zeros(inImage.rows, inImage.cols, CV_8UC1);
	if(inImage.channels() > 1)  
	{
		// Image is not already a grayscale image, so convert it
		 h_rgb2Gray(inImage,grayImage,exTime);
		cout << "\n****************************************" <<endl;
		cout << "CPU_Grayscale Computation time : " <<exTime*1000 << " us" <<endl;
	}
	
	else
		grayImage = inImage.clone();

	//---------------------------------------------------------//
	// Call a function to normalize the grayscale image
	h_normalize(grayImage,normImage,exTime);
	cout << "CPU_Normalization Computation time : " <<exTime*1000 << " us" <<endl;

#endif //CPU_EX
//*******************************************************************************
	//////////////////////////////////////////////////////////////
 	// GPU Functions
	//////////////////////////////////////////////////////////////
#ifdef GPU_EX
	//-----------------------------------------------------------//
	// Convert RBG image to Grayscale
	char message[] = "GPU";
	Mat grayImage = Mat::zeros(inImage.rows, inImage.cols, CV_8UC1);
	Mat normImage = Mat::zeros(inImage.rows, inImage.cols, CV_8UC1);
	const int rows = (const int) inImage.rows;
	const int cols = (const int) inImage.cols;
        if(inImage.channels() > 1)  
	{	
		// Image is not already a grayscale image, so convert it
		k_rgb2Gray((unsigned char*)inImage.data,(unsigned char*)grayImage.data,exTime,rows,cols);
		cout << "\n****************************************" <<endl;
		cout << "GPU_Grayscale Computation time : " <<exTime*1000 << " us" <<endl;
	}
	else
	 	grayImage = inImage.clone();
	//-----------------------------------------------------------//
	// Call a function to normalize the grayscale image
	//Get the minimum and maximum pixel intensity on the image
    	double minVal, maxVal;
	minMaxLoc(grayImage, &minVal, &maxVal);
 	k_normalize((unsigned char*)grayImage.data,(unsigned char*)normImage.data,exTime,rows,cols,minVal,maxVal);
	cout << "GPU_Normalization Computation time : " <<exTime*1000 << " us" <<endl;

#endif //GPU_EX	

//****************************************************************************
#ifdef OCV_EX
	////////////////////////////////////////////////////////////
	// Use OpeCV routines to perform the image processing
	///////////////////////////////////////////////////////////
	char message[] = "OPENCV";
	//Start the CPU clock
	CPUTimer cpuClk;
	//cpuClk.Start();
	// Convert the input RGB to Grayscale
	Mat grayImage;	
	ocv_rgb2Gray(inImage,grayImage,exTime);
	cout << "OpenCV_RGB-to-Grayscale Computation time : " <<exTime*1000 << " us" <<endl;
	
	//Normalize the image
	Mat normImage;
	ocv_normalize(grayImage,normImage,exTime);
	cout << "OpenCV_Normalization Computation time : " <<exTime*1000 << " us" <<endl;
#endif
//****************************************************************************
#ifdef CASCADE
	////////////////////////////////////////////////////////////
	// Call Haar Cascade Classifier
	////////////////////////////////////////////////////////////
	// Time for cascade classifier

	detectAndDisplay(normImage,inImage);
	// Stop the clock 
	gpuClk.Stop();
        exTime = gpuClk.Elapsed(); // Record the time elapsed	
	cout << "Total time for Recognition ( " <<message << " ) :"  <<exTime << " ms" <<endl;
#endif
#ifdef DISPLAY	
	///////////////////////////////////////////////////////////////////
	// Display images
	////////////////////////////////////////////////////////////////////
	if(~inImage.empty())
		showImage(inImage,"ORIGINAL IMAGE");
	if(~grayImage.empty())	
		showImage(grayImage,"GRAYSCALE IMAGE");
	if(~normImage.empty())
		showImage(normImage, "NORMALIZED IMAGE");
	waitKey(0);
#endif

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

