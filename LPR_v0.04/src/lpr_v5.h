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

#ifndef LPR_V4_JEFBY_H
#define LPR_V4_JEFBY_H



//	opencv 2.0 header file
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
//	for function printf 
#include <iostream>
#include <stdio.h> 
#include <vector>
#include <cmath>


using namespace cv;
using namespace std;


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


//Function declare and implements in file lpr_v4.cpp

int remove_Border_Vertical(Mat & img,int &up_border,int &down_border);
int remove_Border_Horizon(Mat & img,int &up_border,int &down_border);

int LPR_Locate(Mat &img_src,Mat &img_dst0,Mat &img_dst1);
int do_lpr_locate(Mat &img_src,Mat &img_dst0,Mat &img_dst1);
int LPR_Tilt(Mat &img_src,Mat & img_src1,Mat &img_dst0,Mat&);
int do_lpr_tilt(Mat &img_src,Mat &img_src1,Mat &img_dst0,Mat&);

int LPR_Segmentation(Mat &img_src0,Mat&img_src1);
int do_lpr_segmentation(Mat &img_src0,Mat &img_src1);
int parting_char(Mat &img);

string convertInt(int number);

#endif
