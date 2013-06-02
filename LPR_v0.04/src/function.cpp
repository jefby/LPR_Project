/*
	*
	*
	*Author : jefby
	*Email : jef199006@gmail.com
	*
*/
#include "lpr_v5.h"

int LPR_Locate(Mat &img_src,Mat &img_dst0,Mat &img_dst1)
{
	do_lpr_locate(img_src,img_dst0,img_dst1);
	namedWindow("clip_gray",CV_WINDOW_NORMAL);
	imshow("clip_gray",img_dst1);
	return 0;
}
int do_lpr_locate(Mat &img_src,Mat &img_dst0,Mat &img_dst1)
{
//	var define 
	Mat img_gray;
	Mat grad_x,grad_y,abs_grad_x,abs_grad_y,grad;
	Mat img_bin_thre;
	int scale = 1;
	int ddepth = CV_16S;
	int delta = 0;
	Mat img_get_rect,img_get_rect_new;
//	0:check the parameter img_dst0 and img_dst1
//	if(!img_dst0.data || !img_dst1.data)
	{
//		printf("parameter img_dst0 or img_dst1 is NULL\n");
//		return -1;
	}

//	1.use Gaussian blur to reduce the noise
	GaussianBlur(img_src,img_src,Size(3,3),0,0,BORDER_DEFAULT);
	

//	2.convert color space of this image to gray 
	cvtColor(img_src,img_gray,CV_BGR2GRAY);

//	3a.sobel check in x direction
	Sobel(img_gray,grad_x,ddepth,1,0,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_x,abs_grad_x);
	
//	3b.sobel check in y direction
	Sobel(img_gray,grad_y,ddepth,0,1,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_y,abs_grad_y);
	
//	3c.sobel add weight
	addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0,grad);
	
//	4.binarition (otsu)
	threshold(grad,img_bin_thre,0,255,THRESH_BINARY | THRESH_OTSU);

//	5.morphology process
	Mat element = getStructuringElement(MORPH_RECT,Size(2*1+1,2*1+1),Point(-1,-1));
	for(int i=0;i<3;++i)
	{
		morphologyEx(img_bin_thre,img_bin_thre,MORPH_OPEN,element);
		morphologyEx(img_bin_thre,img_bin_thre,MORPH_CLOSE,element);
	}
	
	
//	6.find contour,in here must use the binarition image
	vector <Vec4i> hierarchy;
	vector < vector<Point> > contours;
	findContours(img_bin_thre,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
	int min_area = 100000;
	int max_area = 300000;
	Rect mRect;
	int tempArea;

	for(int i = 0; i < contours.size();++i)
	{
//	get the minimum rectangle of the contours
		mRect = boundingRect(contours[i]);
//	computer the square of mRect
		tempArea = mRect.height*mRect.width;	
//	filter area which meet the requirement
		if(((double)mRect.width/(double)mRect.height) > 2.0 && (tempArea > min_area) && \
			((double)mRect.width/(double)mRect.height < 4) && (tempArea < max_area))
//	draw contours
		{
//			drawContours(drawing,contours,i,color,2,8,hierarchy);
//	here use 2 image ,one is just from image which be processed by threshold,the other is the original gray image,if you just use first,you 
//	may not 
			getRectSubPix(img_bin_thre,Size(mRect.width,mRect.height),Point(mRect.x+mRect.width/2,mRect.y\
					 +mRect.height/2),img_dst0);	
			getRectSubPix(img_gray,Size(mRect.width,mRect.height),Point(mRect.x+mRect.width/2,mRect.y\
					 +mRect.height/2),img_dst1);
	
		
			if(img_dst0.data == NULL)
			{
				printf("img_get_rect is NULL \n");
				return -1;
			}
			if(img_dst1.data == NULL)
			{
				printf("img_get_rect_new is NULL \n");
				return -1;
			}
		}
	}	
	
//	check the img_get_rect and img_get_rect_new 's value
	
}


int LPR_Tilt(Mat &img_src,Mat & img_src1,Mat &img_dst0,Mat &img_dst1)
{
	do_lpr_tilt(img_src,img_src1,img_dst0,img_dst1);
	return 0;
}

int do_lpr_tilt(Mat &img_src,Mat &img_src1,Mat &img_dst0,Mat &img_dst1)
{
	Mat img_lines;
//	use the HoughLinesP

//	define lines
	vector<Vec4i> lines;
//	Mat color_dst;
//	img_lines = img_get_rect.clone();
	cvtColor(img_src,img_lines,CV_GRAY2BGR);
//	check the line in image img_get_rect
	HoughLinesP(img_src,lines,1,CV_PI/180,200,200,10);
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
//	drawing the tilt line
	line(img_lines,res.v0,res.v1,Scalar(255,255,0),1);


	Mat img_lines_out;
	Point center = Point(img_lines.cols/2,img_lines.rows/2);
	double angle =(double)(180/CV_PI)*(double)atan(res.slope);
	printf("angle is :%f\n",angle);
	Mat rot_mat = getRotationMatrix2D(center,angle,1.0);
	warpAffine(img_src,img_dst0,rot_mat,img_dst0.size());
//	Mat img_rect;
	warpAffine(img_src1,img_dst1,rot_mat,img_dst1.size());

//	cvtColor(img_dst0,img_dst0,CV_BGR2GRAY);
//	printf("img_clip's channel is:%d\n",img_lines_out.channels());
	threshold(img_dst0,img_dst0,0,255,THRESH_BINARY | THRESH_OTSU);
	threshold(img_dst1,img_dst1,0,255,THRESH_BINARY | THRESH_OTSU);
	namedWindow("tilt_win",CV_WINDOW_NORMAL);
	imshow("tilt_win",img_dst1);
	return 0;
}
int LPR_Segmentation(Mat &img_src0,Mat&img_src1)
{
	do_lpr_segmentation(img_src0,img_src1);
	return 0;
}
int do_lpr_segmentation(Mat &img_src0,Mat &img_src1)
{
	Mat img_clip;
	int up,down;

	if(-1 != remove_Border_Vertical(img_src0,up,down))
	{
		printf("up=%d,down=%d\n",up,down);
		getRectSubPix(img_src0,Size(img_src0.cols,down-up),Point(img_src0.cols/2,up+(down-up)/2),img_clip);
//		namedWindow("line_clip",CV_WINDOW_NORMAL);
//		imshow("line_clip",img_clip);
		getRectSubPix(img_src1,Size(img_src1.cols,down-up),Point(img_src1.cols/2,up+(down-up)/2),img_clip);
		namedWindow("new_clip",CV_WINDOW_NORMAL);
		imshow("new_clip",img_clip);
	}
//	binarition OTSU
	threshold(img_clip,img_clip,0,255,THRESH_BINARY | THRESH_OTSU);
	namedWindow("newrect",CV_WINDOW_NORMAL);
	imshow("newrect",img_clip);

	parting_char(img_clip);
	return 0;
}

//	@function 
//	removal of boarder up and down border

int remove_Border_Vertical(Mat & img,int &up_border,int &down_border)
{
//	FILE *fp = fopen("../img_bit.txt","w");
	if(!img.data)
	{
		printf( "img is invalid!\n");
		return -1;
	}
//	initialize data
	up_border = 0;
	down_border = 0;

	if(1 != img.channels())
	{
		printf("channels is not 1\n");
		return -1;
	}
	
	int nRows = img.rows;
	int nCols = img.cols;
	int cnt_col = 0;

//	
	printf("nRows=%d,nCols=%d\n",nRows,nCols);
	int i,j;
	uchar *p;

	for(int i=0;i<nRows;++i)
	{
		cnt_col = 0;
		p = img.ptr<uchar>(i);
		for(int j = 0; j < nCols-1;++j)
		{
//			fprintf("%x ", p[j]);
			if(p[j] != p[j+1])
				++cnt_col;

		}
		if(cnt_col > 50)
		{
			up_border = i;
			break;
		}

	}

	for(int i = nRows - 1;i >= 0;--i)
	{
		cnt_col = 0;
		p = img.ptr<uchar>(i);
		for(int j=0;j<nCols-1;++j)
		{
			if(p[j] != p[j+1])
				++cnt_col;
		}
		if(cnt_col > 56)
		{
			down_border = i;
			break;
		}
	}


	return 0;
	
}

/*
	*
	*	@function remove the left and right border
	*
*/
int remove_Border_Horizon(Mat & img,int &up_border,int &down_border)
{
	return 0;
}
/*
	*	@function convert int to string 
	*	parameter : int
	* 	return : string
*/

string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
/*
	*
	*	@function partion char set
	*
*/

int parting_char(Mat &img)
{
	int nRows;
	int nCols;
	int nCnt;
	int sum;
	int avg;
	int start;
	Mat img_clip;
	static int card_id;
	

	if(img.channels() != 1)
	{
		printf("parting_char(Mat &img) channel is not 1\n");
		return -1;
	}
	nRows = img.rows;
	nCols = img.cols;
	int *h_Cnt = new int[nCols]; 
	memset(h_Cnt,0,sizeof(int)*nCols);
	for(int j=0;j<nCols;++j)
	{	
		for(int i=0;i<nRows;++i)
		{
			if(img.at<uchar>(i,j) != 0x0)
				++h_Cnt[j];
				
		}
	}

	//getRectSubPix(img,Size(img.width,),Point(img_rect.cols/2,up+(down-up)/2),img_clip);
	for(int i =0;i<nCols;++i)
	{
		if(h_Cnt[i] != 0)
		{
//			printf("i=%d,value=%d",i,h_Cnt[i]);
			start = i;
			nCnt = 0;
			while(i<nCols&&h_Cnt[i] != 0)
			{
				++nCnt;
				++i;
			}
			sum = 0;
			for(int j =0;j<nCnt;++j)
			{
				sum += h_Cnt[i+j];
			}
			avg = sum / nCnt;
			if(avg >= 3)
			{
				
				printf("start=%d,end=%d\n",start,i);
				getRectSubPix(img,Size(i-start,img.rows),Point(start+(i-start)/2,img.rows/2),img_clip);
				if(card_id > 0 && card_id < 8)
				{
					string str = "Card_char"+convertInt(card_id);
					namedWindow(str,CV_WINDOW_NORMAL);
					imshow(str,img_clip);
				}
				++card_id;
			}
		}
	}
	
	delete h_Cnt;
	return 0;
}

