#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include "highgui.h"
using namespace cv;
using namespace std;
//�趨��ֵ��ʹ�õ�����
int DetectThreshold(IplImage* src);
//��ֵ������
Mat toBinary(Mat src, int threshold);
Mat toLog(Mat src);
Mat toGamma(Mat src, float gamma);
Mat RGBtoCMY(Mat src);


int main(int argc, char** argv) {
	//һ��ͼ����ʾ������1
	Mat src, dst;
	src = imread("D:/workspace/images/smallCat.jpg", 1); //��ͼ���Բ�ɫͼ����
	if (!src.data) {
		printf("could not load image...\n");
		return -1;

	}
	cout << "ͼ���ͨ����: " << src.channels() << endl;
	Mat dst_smallize;
	namedWindow("input window", 0);
	imshow("input window", src);
	

	/*������ֵ������
	*��ֵѡȡ������
	*1��˫�巨
	*2��P������
	*3�����
	*4���������ֵ��
	*5����������
	**/
	//int thresh1 = DetectThreshold(cvLoadImage("D:/workspace/images/cat01.jpg"));
	Mat gray;
	cvtColor(src, gray,COLOR_BGR2GRAY);  //ת��Ϊ�Ҷ�ͼ��Ȼ���ֵ��
	namedWindow("gray", 0);
	imshow("gray", gray);
	dst = toBinary(gray, 179);
	//threshold(gray, dst, 100, 200, THRESH_BINARY_INV);
	namedWindow("��ֵͼ", 0);
	imshow("��ֵͼ", dst);


	/*
	*�����Ҷ�ͼ������任��
	*	���ò�����rֵ��ͼ����ж����任
	*	��ʽ��s = log��1+r��
	*/
	Mat result = toLog(src);
	namedWindow("�����任", 0);
	imshow("�����任", result);


	/*
	*�ġ�٤��任
	*	���ò�����٤��ֵ��ͼ�����٤��任
	*
	*/
	Mat result_gamma = toGamma(src, 3.0);
	namedWindow("٤��任", 0);
	imshow("٤��任", result_gamma);

	/*
	*�塢��ɫ�任
	*	�Բ�ɫͼ����в�ɫ�任
	*	��ʽ s = 255-i
	*/
	Mat result_cmy = RGBtoCMY(src);
	namedWindow("��ɫ�任", 0);
	imshow("��ɫ�任", result_cmy);

	while (1) {
		if (cvWaitKey(100) == 27)break;
	}
	waitKey(0);
	exit(0);
}
Mat toBinary(Mat src, int threshold) {
	Mat result;
	int channels = src.channels();
	int rows = src.rows;
	int cols = src.cols*channels;
	int j;
	uchar* p;
	uchar* r;
	p = src.ptr<uchar>(0);
	r = result.ptr<uchar>(0);
	for (j = 0; j < cols*rows; j++) {
		if (p[j] > threshold) {
			p[j] = 255;
		}
		else {
			p[j] = 0;
		}
	}
	return src;
}

Mat toLog(Mat src) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = log(1 + i);

	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];

		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

Mat toGamma(Mat src, float gamma) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = pow(i, gamma);
	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];
		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

Mat RGBtoCMY(Mat src) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = 255 - i;
	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];
		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

//������
int DetectThreshold(IplImage* src) {
	uchar iThrehold;//��ֵ
	try {
		int height = src->height;
		int width = src->width;
		int step = src->widthStep / sizeof(uchar);//�����е�ͬ�е�֮����ֽ���
		uchar *data = (uchar*)src->imageData;

		int iDiffRec = 0;
		int F[256] = { 0 }; //ֱ��ͼ����  
		int iTotalGray = 0;//�Ҷ�ֵ��  
		int iTotalPixel = 0;//��������  
		uchar bt;//ĳ�������ֵ  

		uchar iNewThrehold;//�·�ֵ
		uchar iMaxGrayValue = 0, iMinGrayValue = 255;//ԭͼ���е����Ҷ�ֵ����С�Ҷ�ֵ  
		uchar iMeanGrayValue1, iMeanGrayValue2;
		//��ȡ(i,j)��ֵ������ֱ��ͼ����F  
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				bt = data[i*step + j];
				if (bt < iMinGrayValue)
					iMinGrayValue = bt;
				if (bt > iMaxGrayValue)
					iMaxGrayValue = bt;
				F[bt]++;
			}
		}
		iThrehold = 0;
		iNewThrehold = (iMinGrayValue + iMaxGrayValue) / 2;//��ʼ��ֵ  
		iDiffRec = iMaxGrayValue - iMinGrayValue;
		for (int a = 0; (abs(iThrehold - iNewThrehold) > 0.5); a++)//������ֹ����  
		{
			iThrehold = iNewThrehold;
			//С�ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ  
			for (int i = iMinGrayValue; i < iThrehold; i++)
			{
				iTotalGray += F[i] * i;//F[]�洢ͼ����Ϣ  
				iTotalPixel += F[i];
			}
			iMeanGrayValue1 = (uchar)(iTotalGray / iTotalPixel);
			//���ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ  
			iTotalPixel = 0;
			iTotalGray = 0;
			for (int j = iThrehold + 1; j < iMaxGrayValue; j++)
			{
				iTotalGray += F[j] * j;//F[]�洢ͼ����Ϣ  
				iTotalPixel += F[j];
			}
			iMeanGrayValue2 = (uchar)(iTotalGray / iTotalPixel);

			iNewThrehold = (iMeanGrayValue2 + iMeanGrayValue1) / 2; //�·�ֵ  
			iDiffRec = abs(iMeanGrayValue2 - iMeanGrayValue1);
		}
	}
	catch (cv::Exception e)
	{
	}

	return iThrehold;
}

/*Mat binary_convert(Mat src, uchar iThreshold) {
	Mat result = src;
	int row = result.rows;
	int col = result.cols;
	for (int i = 0; i <= row; i++) {
		for (int j = 0; j <= col; j++) {
			result[i][j] = result[i][j] > iThreshold ? 255 : 0;
		}
	}
	return result;
}*/

