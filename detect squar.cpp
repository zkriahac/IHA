#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	VideoCapture cap(0); //capture the video from webcam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int RLowH = 0;
	int RHighH = 179;

	int RLowS = 0;
	int RHighS = 255;

	int RLowV = 0;
	int RHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &RLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &RHighH, 179);

	createTrackbar("LowS", "Control", &RLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &RHighS, 255);

	createTrackbar("LowV", "Control", &RLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &RHighV, 255);

	
 //blue color
/*
//blue color
	int BLowH = 75;
	int BHighH = 130;

	int BLowS = 133;
	int BHighS = 255;

	int BLowV = 196;
	int BHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &BLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &BHighH, 179);

	createTrackbar("LowS", "Control", &BLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &BHighS, 255);

	createTrackbar("LowV", "Control", &BLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &BHighV, 255);

	/////////////////////////////
*/
 //yallow color
/*
	int YLowH = 22;
	int YHighH = 38;

	int YLowS = 150;
	int YHighS = 255;

	int YLowV = 150;
	int YHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &YLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &YHighH, 179);

	createTrackbar("LowS", "Control", &YLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &YHighS, 255);

	createTrackbar("LowV", "Control", &YLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &YHighV, 255);
*/
	

	

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

	iLastX = -1;
	iLastY = -1;
	while (true)
	{
		Mat imgOriginal;
		
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;


		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(RLowH, RLowS, RLowV), Scalar(RHighH, RHighS, RHighV), imgThresholded); //Threshold the image

		 //morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 100000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a red line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
				//rectangle(imgLines, Point(posX, posY), Point(posX + 60, posY + 60), Scalarr(0, 255, 0), 5);
			}

			iLastX = posX;
			iLastY = posY;
		}

		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}