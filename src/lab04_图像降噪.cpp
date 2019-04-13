#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;
void getZ(const Mat &roi, int &Z_min, int &Z_max, int &Z_med) {
	vector<uchar> vec;
	for (auto iter = roi.begin<uchar>(); iter != roi.end<uchar>(); ++iter) {
		vec.push_back(*iter);
	}
	sort(vec.begin(), vec.end());

	Z_min = vec.front();
	Z_max = vec.back();
	Z_med = vec[roi.rows * roi.cols / 2];
}

Mat adaptiveMedian(const Mat &srcImg, int n_max) {
	Mat retImg = srcImg.clone();
	int rows = srcImg.rows;
	int cols = srcImg.cols;

	for (int x = n_max / 2; x < rows - n_max / 2; ++x) {
		for (int y = n_max / 2; y < cols - n_max / 2; ++y) {
			//ÿ�����ض���ģ��Ϊ3��ʼ
			for (int n = 3; n <= n_max; n += 2) {
				int k = n / 2;
				Mat roi = srcImg(Range(x - k, x + k), Range(y - k, y + k));
				int Z_min, Z_max, Z_med;
				int Z_xy = srcImg.at<uchar>(x, y);
				getZ(roi, Z_min, Z_max, Z_med);

				//�ﵽ�����, ֱ�Ӹ�ֵZ_med
				if (n == n_max) {
					retImg.at<uchar>(x, y) = Z_med;
					break;
				}

				//
				if (Z_med - Z_min == 0 || Z_med - Z_max == 0) { //��ֵ������(Ϊ������Сֵ), ���Ӵ���
					continue; // ����n
				}
				else {    //��ֵ��������ʱ�����е��ֵ
					if (Z_xy - Z_min > 0 && Z_xy - Z_max < 0) //���ĵ㲻������, ����
						retImg.at<uchar>(x, y) = Z_xy;
					else
						retImg.at<uchar>(x, y) = Z_med;
					break;
				}
			}
		}
	}
	return retImg;
}


//����ƽ��
Mat meanFilter(Mat src, int moudelInt);
//����ƽ��
Mat geometricFilter(Mat src, int moudelInt);
//г����ֵ
Mat harmonicFilter(Mat src, int moudelInt);
//��г����ֵ Q���˲������� |Q=0 г����ֵ |Q=-1 ������ֵ |QΪ���������������� Ϊ���������������� 
Mat In_harmonicFilter(Mat src, int moudelInt, double Q);
//��ֵ�˲�
Mat medianFilter(Mat src, int moudelInt);
//����Ӧ��ֵ�˲� overall�����ķ���
Mat adaptiveMeanFilter(Mat src, int moudelInt, double overall);
//����Ӧ��ֵ�˲� maxMoudel ��󴰿ڴ�С 7
Mat adaptiveMedianFilter(Mat src, int moudelInt);
//����ֵ n ģ���С
int getMedian(int n, int* array);


int main(int argc, char** argv)
{

	Mat src_g = imread("D:/workspace/images/saltnoise.jpg", 0);//�Ҷ�ͼ��
	Mat src_color = imread("D:/workspace/images/saltnoise.jpg", 1);//��ɫͼ��
	//����Ҷ�ͼ��
	namedWindow("src_grey", 1);
	imshow("src_grey", src_g);

	//������ֵ
	Mat src_mean = meanFilter(src_g, 7);
	namedWindow("mean", 1);
	imshow("mean", src_mean);
	//���ξ�ֵ
	//Mat src_geome = geometricFilter(src_g, 5);
	//namedWindow("geometric", 1);
	//imshow("geometric", src_geome);
	//г��
	Mat src_har = harmonicFilter(src_g, 5);
	namedWindow("harmonic", 1);
	imshow("harmonic", src_har);
	//��г�� In_harmonicFilter(Mat src,int moudelInt,double Q); 
	//Mat src_Inhar = In_harmonicFilter(src_g, 7, 0);
	//namedWindow("inhar", 1);
	//imshow("inhar", src_Inhar);
	//��ֵ�˲�  medianFilter
	Mat src_median = medianFilter(src_g, 7);
	namedWindow("median", 1);
	imshow("median", src_median);
	imwrite("../median.jpg", src_median);

	//����Ӧ��ֵ�˲� overall�����ķ��� Mat adaptiveMeanFilter(Mat src,int moudelInt,double overall);
	Mat src_adMean = adaptiveMedian(src_g,  7);
	//Mat src_adMean = adaptiveMeanFilter(src_g, 7, 0.1);
	namedWindow("adMean", 1);
	imshow("adMean", src_adMean);
	//����Ӧ��ֵ�˲� Mat adaptiveMedianFilter(Mat src,int moudelInt);
	//Mat src_adMedian = adaptiveMedianFilter(src_g, 7);
	//namedWindow("adMedian", 1);
	//imshow("adMedian", src_adMedian);

	waitKey(0);
	return 0;
}


/************************************************A P I***********************************************************/
//Mat���ţ�����ģ���ھ���[0,0]�Լ����Ʊ�Եλ��ʱ�޷�����
Mat expandsrc(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}
	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	int expand = 2 * controller;
	//�������Mat ����Ա�Ե���в���
	//��src_clone�ⲿ���ϼ��м���
	int r = nRows + expand;
	int c = nCols + expand;
	Mat src_expand = Mat::zeros(r, c, CV_8UC1);//��ʼ������Mat��ʼֵ0
	int x, y = 0;
	for (x = 0; x<nRows; ++x)
	{
		for (y = 0; y<nCols; ++y)
		{
			src_expand.at<uchar>(x + controller, y + controller) = src_clone.at<uchar>(x, y);
		}
	}
	//�����Ƕ�׵�0ȫ����Ϊ�ͱ߿���ͬ��ֵ
	//todo......

	return src_expand;

}



//src Դͼ�� moudelIntģ���С Ϊ����
//������ֵ�˲� ģ���С5��5 Ϊ�˻�ýϺõ�ȥ��Ч�����鲻Ҫ����С
Mat meanFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//�����ڲ�С����
			int sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum += src_expand.at<uchar>(inneri, innerj);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum / (moudelInt*moudelInt);
			sum = 0;
		}
	}
	return src_clone;
}


//���ξ�ֵ�˲� ģ���С�̶�5��5
Mat geometricFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//�����ڲ�С����
			double sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum *= pow(src_expand.at<uchar>(inneri, innerj), 1.0 / 25.0);//����ļ����ж�ʧ ���ȫ����0
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum;
			sum = 0;
		}
	}
	return src_clone;
}

//г����ֵ�˲� ģ���С5��5 Ϊ�˻�ýϺõ�ȥ��Ч�����鲻Ҫ����С
//�ʺϴ��������� ���ʺϺ�������
Mat harmonicFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//�����ڲ�С����
			double sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum += 1.0 / src_expand.at<uchar>(inneri, innerj);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = 25 / sum;
			sum = 0;
		}
	}
	return src_clone;
}
//��г����ֵ Q���˲�������
Mat In_harmonicFilter(Mat src, int moudelInt, double Q)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//�����ڲ�С����
			double sum1 = 0;
			double sum2 = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum1 += pow(src_expand.at<uchar>(inneri, innerj), Q);
					sum2 += pow(src_expand.at<uchar>(inneri, innerj), Q + 1);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum2 / sum1;
			sum1 = 0;
			sum2 = 0;
		}
	}
	return src_clone;
}
//��ֵ�˲� ģ���С5��5 7��7
Mat medianFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	int size = moudelInt*moudelInt;
	int array[49] = { 0 };
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//Ѱ���ڲ�С�������ֵ

			int row = 0;
			int col = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{

					row = inneri - i + controller;
					col = innerj - j + controller;

					int temp = src_expand.at<uchar>(inneri, innerj);


					array[row*moudelInt + col] = src_expand.at<uchar>(inneri, innerj);	//�������������
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = getMedian(moudelInt, array);//����ֵ
		}
	}
	return src_clone;
}


//����Ӧ��ֵ�˲� ģ���С7��7 overall������ȫ�ַ���
Mat adaptiveMeanFilter(Mat src, int moudelInt, double overall)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	Mat src_expand = expandsrc(src_clone, moudelInt);
	//��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			////����Ml �ֲ���ֵ �����ľֲ����� ���صľֲ�����
			double average = 0;
			double variance = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{

					//�ֲ���ֵMl
					average += src_expand.at<uchar>(inneri, innerj) / (moudelInt*moudelInt);

				}
			}
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					//�ֲ�����
					int temp = (src_expand.at<uchar>(inneri, innerj) - average);
					variance += pow(temp, 2) / (moudelInt*moudelInt);

				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = src_clone.at<uchar>(i, j) - (overall / variance)*(src_clone.at<uchar>(i, j) - average);
		}
	}
	return src_clone;
}
//�߼��ܻ��� �������Ѿ��Ĳ�����
//����Ӧ��ֵ�˲�  ģ������С �̶�Ϊ7
Mat adaptiveMedianFilter(Mat src, int moudelInt)
{
	//��Ϊģ�������δ֪������һ��ʼԭʼͼ�������Ҫ��ģ������ֵ������
	Mat src_c = src.clone();
	if (!src_c.data)
	{
		cout << "��ȡͼ��ʧ�ܣ�" << endl;
		//return ʧ����Ϣ����ֹ����ִ��
	}

	int nRows = src_c.rows;
	int nCols = src_c.cols;
	int windowSize = 7;
	Mat src_expand = expandsrc(src_c, 7);//�������ģ���С����
	Mat src_clone = src_expand;//clone������expand��Сһ�� ����д��clone

							   //��src_expand���м��㣬�Ѽ�����д��src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	//int count = 0;//���� ����
	int row = 0;
	int col = 0;
	int min = 0;
	int max = 0;
	int median = 0;

	for (i = 3; i<nRows + 3; ++i)
	{
		for (j = 3; j<nCols + 3; ++j)
		{
		dy_expand:
			int controller = windowSize / 2;
			if (windowSize <= 7) {
				//��̬�任�ڲ�С����Ĵ�С
				int length = windowSize*windowSize;
				int array[49] = { 0 };
				int current = src_expand.at<uchar>(i, j);
				for (inneri = i - controller; inneri <= i + controller; ++inneri)
				{
					for (innerj = j - controller; innerj <= j + controller; ++innerj)
					{

						//cout<<"inneri:innerj"<<inneri<<":"<<innerj<<endl;
						row = inneri - i + controller;
						col = innerj - j + controller;
						int temp = src_expand.at<uchar>(inneri, innerj);
						//cout<<"row:"<<row<<"col:"<<col<<endl;
						//cout<<"value"<<temp<<endl;
						array[row*windowSize + col] = src_expand.at<uchar>(inneri, innerj);	//�������������	
					}
				}
				//Ѱ��arry�����ֵ ��Сֵ ��ֵ	
				int i1 = 0;
				int j1 = 0;
				for (i1 = 0; i1<length; ++i1)
				{
					for (j1 = i1 + 1; j1<length; ++j1)
					{
						if (array[j1]<array[i1])
						{
							int temp = 0;
							temp = array[i1];
							array[i1] = array[j1];
							array[j1] = temp;
						}
					}
				}
				min = array[0];
				max = array[length - 1];
				median = array[length / 2];
				current = src_expand.at<uchar>(i, j);

				//ģ�����Ϊ7��7
				/*
				A: A1=med-min
				A2=med-max
				if(A1>0A2<0) ת��B �������󴰿ڳߴ�
				if���ڳߴ�С�ڵ������ֵ �ظ�A �������med
				B: B1=current-min
				B2=current-max
				if(B1>0B2<0)���current
				else���med
				*/
				if (median - min>0 && median - max<0)
				{
					//cout<<"��ֵ��min��max֮�䡪��>B"<<endl;
					//STAR B----------------------------------------
					if (current - min>0 && current - max<0)
					{
						//cout<<"min<current<max current"<<endl;
						src_clone.at<uchar>(i, j) = current;//д���λ��Ҫ�����
						windowSize = 3;

					}
					else {
						//cout<<"current����[min,max] median"<<endl;
						src_clone.at<uchar>(i, j) = median;
						windowSize = 3;

					}
					//END B-----------------------------------------
				}
				else if (windowSize <= 5) {
					//cout<<"��ֵ����min��max֮��"<<endl;

					//��������
					windowSize = windowSize + 2;
					goto dy_expand;

				}
				else {
					//cout<<"��������� ���median"<<endl;
					src_clone.at<uchar>(i, j) = median;
					windowSize = 3;
				}
			}
		}
	}
	return src_clone;
}

//�Թ�ģΪn�ľ������� ��������ֵ
int getMedian(int n, int* array)
{
	int length = n * n;
	int i = 0;
	int j = 0;
	for (i = 0; i<length; ++i)
	{
		for (j = i + 1; j<length; ++j)
		{
			if (array[j]<array[i])
			{
				int temp = 0;
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
	int median = array[length / 2];
	return median;
}





