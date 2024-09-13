/*Author: Stan Yin
* GitHub Name: SomeB1oody
* This project is based on CC 4.0 BY, please mention my name if you use it.
* This project requires opencv.
*/
#include<bits/stdc++.h>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main()
{
	string location;
	cout << "Please enter image's location" << endl;
	cout << "Example: C:\\Wallpaper\\Picture.jpg" << endl;
	cin >> location;
	Mat image = imread(location, IMREAD_COLOR);
	if (image.empty())
	{
		cout << "Fail to read image" << endl;
		return -1;
	}
	Mat smallImage;
	resize(image, smallImage, Size(708, 708));
	Mat constrast = Mat::zeros(smallImage.size(), smallImage.type());
	int length = 708;
	Mat process;
	smallImage.convertTo(process, 5);
	int alpha = 1.5, beta = -25;
	for (int row = 0; row < length; row++)
	{
		for (int col = 0; col < length; col++)
		{
			float red = process.at<Vec3f>(row, col)[2];
			float green = process.at<Vec3f>(row, col)[1];
			float blue = process.at<Vec3f>(row, col)[0];

			constrast.at<Vec3f>(row, col)[2] = saturate_cast<uchar>(red * alpha + beta);
			constrast.at<Vec3f>(row, col)[1] = saturate_cast<uchar>(green * alpha + beta);
			constrast.at<Vec3f>(row, col)[0] = saturate_cast<uchar>(blue * alpha + beta);
		}
	}
	Mat grayImage;
	cvtColor(constrast, grayImage, COLOR_BGR2GRAY);
	Mat filter;
	bilateralFilter(grayImage, filter, 14, 28, 4);
	Mat binary;
	threshold(filter, binary, 200, 255, THRESH_BINARY_INV);
	Mat canny;
	Canny(binary, canny, 10, 100, 3, false);
	vector<Vec2f>hLines, nlines;
	HoughLines(canny, hLines, 5, CV_PI / 180, 100);
	double argument1 = 50.00, argument2 = CV_PI / 9;
	set<size_t>rIndex;
	float rho1, rho2, theta[2] = {};
	bool sign, sign2, sign3 = true;
	while (sign3)
	{
		for (size_t index1 = 0; index1 < 704; index1++)
		{
			for (size_t index2 = 0; index2 < 704; index2++)
			{
				rho1 = hLines[index1][0], theta[0] = hLines[index1][1];
				rho2 = hLines[index2][0], theta[1] = hLines[index2][1];
				for (int index3 = 0; index3 < 2; index3++)
				{
					if (theta[index3] > CV_PI)
					{
						theta[index3] -= CV_PI;
					}
				}
				if (abs(theta[0] - theta[1]) <= argument2)
				{
					sign = true;
				}
				if ((theta[0] > CV_PI / 2) && (theta[1] < CV_PI / 2) && (CV_PI - theta[1] + theta[0] < argument2))
				{
					sign = true;
				}
				if ((theta[1] > CV_PI / 2) && (theta[0] < CV_PI / 2) && (CV_PI - theta[1] + theta[0] < argument2))
				{
					sign = true;
				}
				if (abs(rho1 - rho2) <= argument1)
				{
					sign2 = true;
				}
				if (sign && sign2)
				{
					rIndex.insert(index2);
				}
			}
		}
		for (size_t index4 = 0; index4 < hLines.size(); index4++)
		{
			if (rIndex.find(index4) == rIndex.end())
			{
				nlines.push_back(hLines[index4]);
			}
		}
		hLines = nlines;
		if (hLines.size() == 4)
		{
			sign3 = false;
		}
	}
	double threshold = 0.2 * 708, rho3, rho4, theta3, theta4, a1, a2, b1, b2, x, y;
	bool flag;
	vector<Point>points;
	for (int index5 = 0; index5 < nlines.size(); index5++)
	{
		for(int index6 = index5 + 1; index6 < nlines.size(); index6++)
		{
			rho3 = nlines[index5][0], theta3 = nlines[index5][1];
			rho4 = nlines[index6][0], theta4 = nlines[index6][1];
		}
		if (theta3 == 0)
		{
			theta3 = 0.01;
		}
		if (theta4 == 0)
		{
			theta4 = 0.01;
		}
		a1 = cos(theta3), a2 = cos(theta4);
		b1 = sin(theta3), b2 = sin(theta4);
		x = (rho4 * b1 - rho3 * b2) / (a2 * b1 - a1 * b2);
		y = (rho1 - a1 * x) / b1;
		Point pt(cvRound(x), cvRound(y));
		if (pt.x <= 708 + threshold)
		{
			flag = true;
		}
		if (pt.x >= -threshold)
		{
			flag = true;
		}
		if (pt.y <= 708 + threshold)
		{
			flag = true;
		}
		if (pt.y >= -threshold)
		{
			flag = true;
		}
		if (flag)
		{
			points.push_back(pt);
		}
	}
	Point2f current[4], change[4];
	Mat transmatrix = getPerspectiveTransform(current, change);
	Mat perImage = Mat::zeros(smallImage.size(), smallImage.type());
	warpPerspective(smallImage, perImage, transmatrix, perImage.size());
	QRCodeDetector detector;
	vector<String> Text;
	vector<Point> pointlist;
	detector.detectAndDecode(perImage, Text, pointlist);
	cout <<endl<<"Decode result:" <<endl << Text[0] << endl;
	return 0;
}