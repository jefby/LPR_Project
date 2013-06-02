#include "lpr_v5.h"


Mat img_src;
Mat img_loc0,img_loc1;
Mat img_tilt0,img_tilt1;

int main(int argc,char*argv[])
{
	if(argc != 2)
	{
		printf("argc != 2,please check the input format!\n");
		return -1;
	}

	img_src = imread(argv[1]);
	namedWindow("original_img",CV_WINDOW_NORMAL);
	imshow("original_img",img_src);

	if(img_src.data == NULL)
	{
		printf("read from argv[1] error!\n");
		return -1;
	}
//	license plate  locate 	
	LPR_Locate(img_src,img_loc0,img_loc1);
	LPR_Tilt(img_loc0,img_loc1,img_tilt0,img_tilt1);
	LPR_Segmentation(img_tilt0,img_tilt1);
	waitKey(0);
	return 0;
	
}
