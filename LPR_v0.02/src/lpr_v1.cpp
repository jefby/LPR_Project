/*
	*This cpp implements the simplest License Plate locate
	*simple image is stored images directory
	*
	*
	*website: http://www.eefocus.com/jefby1990/blog/
	*email: jef1990@gmail.com
	*Author : jefby
	*Date:	2013/5/27
*/
//	opencv 2.0 header file
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//	#include "opencv2/core/core.hpp"
//	for function printf 
#include <stdio.h> 
//	#include <map>
#include <vector>
#include <cmath>
//	#include <iostream>	

using namespace std;
using namespace cv;

class dou_int{
//Data define,just for simple,so declares it to public
	public:
		double slope;
		int maxlength;
		int num;
		Point v0;
		Point v1;
//Function define
	public:
		dou_int():slope(0),maxlength(0),num(0),v0(0,0),v1(0,0){}
		dou_int(double s,int m,int n,Point x,Point y):slope(s),maxlength(m),num(n),v0(x),v1(y){}
		dou_int(const dou_int & rhs) {
//		if(rhs != this)
		{
			this->slope = rhs.slope;
			this->maxlength = rhs.maxlength;
			this->num = rhs.num;
			this->v0 = rhs.v0;
			this->v1 = rhs.v1;
		}
	}
		dou_int & operator=(const dou_int & rhs)
		{
			this->slope = rhs.slope;
			this->maxlength = rhs.maxlength;
			this->num = rhs.num;
			this->v0 = rhs.v0;
			this->v1 = rhs.v1;
			return *this;
		}
		~dou_int(){}
	};

//	some variable defines 
//	img_src defines the picture which reads through imread
//	img_gray is gray image of img_src
//	grad_x is the image which uses the sobel grad in x direction
//	grad_y is the image which uses the sobel grad in y direction
//	grad is the mixture of 0.5 grad_x and 0.5 grad_y
//	img_bin_thre is the picture which use the OTSU threshold binarization 
Mat img_src;
Mat img_gray;
Mat grad_x,grad_y,abs_grad_x,abs_grad_y,grad;
//	Mat grad;
Mat img_bin_thre;
Mat img_get_rect;
Mat img_lines;


int main(int argc,char**argv)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
//	check the number of parameter
	if(argc !=2)
	{	
		printf("please follow like this\n");
		printf("exe[] img_name\n");
		return -1;
	}
//	img_src = imread(argv[1],CV_LOAD_IMAGE_COLOR);
//	reads image
	img_src = imread(argv[1]);
//	check whether read operation is ok or not 
	if(img_src.data == NULL)
	{	
		printf("could not open or find the image!\n");
		return -1;
	}
//	namedWindow("src Windows",CV_WINDOW_NORMAL);
//	namedWindow("gray Windows",CV_WINDOW_NORMAL);
//	imshow(const string & winname,InputArray mat)
//	show the original image
//	imshow("src Windows",img_src);
//	convert to the gray 
//	cvtColor(img_src,img_gray,CV_BGR2GRAY);
//	show the gray image
//	imshow("gray Windows",img_gray);

//	use Gaussian blur to reduce the noise
	GaussianBlur(img_src,img_src,Size(3,3),0,0,BORDER_DEFAULT);
//	namedWindow("Gaussian",CV_WINDOW_NORMAL);
//	show the image after gaussion blur 
//	imshow("Gaussian",img_src);
//	wait for user 
//	waitKey(0);

//	convert source image to gray image
	cvtColor(img_src,img_gray,CV_BGR2GRAY);
//	sobel in x direction
	Sobel(img_gray,grad_x,ddepth,1,0,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_x,abs_grad_x);
//	show the result	
//	namedWindow("GradientX",CV_WINDOW_NORMAL);
//	imshow("GradientX",abs_grad_x);

//	use sobel in y direction
	Sobel(img_gray,grad_y,ddepth,0,1,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_y,abs_grad_y);
//	namedWindow("GradientY",CV_WINDOW_NORMAL);
//	show the result
//	imshow("GradientY",abs_grad_y);
//	add weight,and 
	addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0,grad);
//	namedWindow("addweight",CV_WINDOW_NORMAL);
//	imshow("addweight",grad);

//	use threshold to binarition
	threshold(grad,img_bin_thre,0,255,THRESH_BINARY|THRESH_OTSU);
//	namedWindow("thresh_otsu",CV_WINDOW_NORMAL);
//	imshow("thresh_otsu",img_bin_thre);
//	first Dilate,second erode
	Mat element = getStructuringElement(MORPH_RECT,Size(2*1+1,2*1+1),Point(1,1));
	dilate(img_bin_thre,img_bin_thre,element);
//	namedWindow("dilated",CV_WINDOW_NORMAL);
//	imshow("dilated",img_bin_thre);
	erode(img_bin_thre,img_bin_thre,element);
//	namedWindow("erode",CV_WINDOW_NORMAL);
//	imshow("erode",img_bin_thre);

//	find contour
//	define 
	vector<Vec4i> hierarchy;
	vector< vector<Point> >contours;
//	use function
	findContours(img_bin_thre,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
//	please change min and the max area value based on reality
	int min_area = 100000;
	int max_area = 300000;
	Rect mRect;
	int tempArea;
//	define the color drawing contour
	Scalar color = Scalar(255,255,0);
	Mat drawing = Mat::zeros(img_bin_thre.size(),CV_8UC1);
	for(int i = 0;i < contours.size();i++)
	{
//	get the minimum rectangle of the contours
		mRect = boundingRect(contours[i]);
//	computer the square of mRect
		tempArea = mRect.height * mRect.width;
//	for debug
//		printf("tempArea.height:%d\ttempArea.width:%d\ttempArea.area=%d\n",mRect.height,mRect.width,tempArea);
//	filter area which meet the requirement
		if(((double)mRect.width/(double)mRect.height) > 2.0 && (tempArea > min_area) && ((double)mRect.width/(double)mRect.height < 4) && (tempArea < max_area))
//	draw contours
		{
			drawContours(drawing,contours,i,color,2,8,hierarchy);
			getRectSubPix(img_bin_thre,Size(mRect.width,mRect.height),Point(mRect.x+mRect.width/2,mRect.y\
					 +mRect.height/2),img_get_rect);	
		}
	}
//	namedWindow("contours",CV_WINDOW_NORMAL);
//	imshow("contours",drawing);
//	namedWindow("getRect",CV_WINDOW_NORMAL);
//	imshow("getRect",img_get_rect);

//	use the HoughLinesP

//	define lines
	vector<Vec4i> lines;
//	Mat color_dst;
//	img_lines = img_get_rect.clone();
	cvtColor(img_get_rect,img_lines,CV_GRAY2BGR);
	HoughLinesP(img_get_rect,lines,1,CV_PI/180,200,200,10);
	printf("lines.size()=%d\n",lines.size());
	
	int distance = 0;
//	int theta;
	double temp_slope = 0,slope;
	int res_x1,res_y1,res_x2,res_y2;
//	define map vector for computer the line used frequency
//	vector <int,int> ivect;//first is the number of this line , next is the longest distance 
//	map <double,ivect> imap;
	int delta_x,delta_y;

	
	std::vector <dou_int> ivec;
	std::vector <dou_int>::iterator iter;

	for(size_t i = 0;i < lines.size();i++)
	{
		Vec4i l = lines[i];
		line(img_lines,Point(l[0],l[1]),Point(l[2],l[3]),Scalar(0,0,255),3);
//	find tilt angle
		if(l[2]-l[0] == 0)
			;
		else
		{
//	computer this line 's slope
//	delta_x / delta_y
			delta_y = (l[3]-l[1]);
			delta_x = (l[2]-l[0]);
			
			distance = delta_y*delta_y+delta_x*delta_x;
			temp_slope = ((double)delta_y)/((double)(delta_x));
			printf("in i=%d,delta_y=%d,delta_x=%d\n",i,delta_y,delta_x);

			for(iter = ivec.begin();iter != ivec.end();iter++)
			{
//	if in one line,num++,update the max length
				if(abs(iter->slope - temp_slope) < (double)0.01)
				{
					iter->num++;
					if(iter->maxlength < distance)
					{
						iter->maxlength = distance;
						iter->v0 = Point(l[0],l[1]);
						iter->v1 = Point(l[2],l[3]);
					}
					break;
				}
			}
//	not find this slope ,must add it by hand 
			if(iter == ivec.end())
			{
				ivec.push_back(dou_int(temp_slope,distance,1,Point(l[0],l[1]),Point(l[2],l[3])));	
			}
		}
	}
int max = 0;
int j = 0;
int index = 0;
dou_int res;

	for(j=0,iter = ivec.begin();iter != ivec.end();j++,iter++)
	{
		if(iter->num > max)
		{
			max = iter->num;
			index = j;
		}
	}
	printf("index is %d\n",index);
	for(j=0,iter = ivec.begin();iter != ivec.end() && j <= index;j++,iter++)
	{
		if(j == index)
		{
			res = dou_int(iter->slope,iter->maxlength,iter->num,iter->v0,iter->v1);
			printf("slope is %f\n",iter->slope);
			break;
		}
	}
	line(img_lines,res.v0,res.v1,Scalar(255,255,0),1);

/*
//	use the function HoughLines
	vector<Vec2f> lines;
//	set threshold to 200
	HoughLines(img_get_rect,lines,1,CV_PI,200,0,0);

	cvtColor(img_get_rect,img_lines,CV_GRAY2BGR);
	for(size_t i = 0 ;i <lines.size();i++)
	{
		float rho = lines[i][0],theta = lines[i][1];
		Point pt1,pt2;
		double a =cos(theta),b=sin(theta);
		double x0 = a*rho,y0=b*rho;
		pt1.x = cvRound(x0+1000*(-b));
		pt1.y = cvRound(y0+1000*a);
		pt2.x = cvRound(x0-1000*(-b));
		pt2.y = cvRound(y0-1000*a);
		line(img_lines,pt1,pt2,Scalar(0,0,255),2);
	}
*/
	Mat img_lines_out;
	Point center = Point(img_lines.cols/2,img_lines.rows/2);
	double angle =(double)(180/CV_PI)*(double)atan(res.slope);
	printf("angle is :%f\n",angle);
	Mat rot_mat = getRotationMatrix2D(center,angle,1.0);
	warpAffine(img_lines,img_lines_out,rot_mat,img_lines.size());

//	just for test
	line(img_lines_out,Point(0,0),Point(img_lines.cols/2,img_lines.rows/2),Scalar(255,0,255),1);

	namedWindow("line_window",CV_WINDOW_NORMAL);
	imshow("line_window",img_lines);

	namedWindow("line_window_out",CV_WINDOW_NORMAL);
	imshow("line_window_out",img_lines_out);
	waitKey(0);
	return 0;
}
