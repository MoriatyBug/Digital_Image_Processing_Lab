#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;
using namespace cv;


//Mat expandsrc(Mat src, int moudelInt);
void Filter(const Mat &src, Mat &dst, int ksize, double** templateMatrix);
void MeanFilter(const Mat &src, Mat &dst, int ksize);
void GaussianFilter(const Mat &src, Mat &dst, int ksize, double sigma);
void Laplacian(const Mat &src, Mat &dst);
void rob_sob(const Mat& src, Mat& dst, int x[][3], int y[][3]);
void enhanceFilter(const Mat& src, Mat& dst, int ksize, double k);
int main(int argc, char* argv[]) {

	Mat src, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
	//@param1 ͼƬ·��
	//@param2 ����ͼƬ��ʽ��0,�Ҷ�ͼ��1,��ɫͼ��
	src = imread("D:/workspace/images/smallCat.jpg", 1);
	if (src.empty()) {
		cout << "�Ҳ���ָ���ļ�" << endl;
		return -1;
	}
	dst1 = src.clone();
	dst2 = src.clone();
	dst3 = src.clone();
	dst4 = src.clone();
	dst5 = src.clone();
	dst6 = src.clone();
	dst7 = src.clone();
	//ԭͼ
	namedWindow("ԭͼ", 0);
	imshow("ԭͼ", src);

	//��ֵ�˲�
	MeanFilter(src, dst1, 9);
	namedWindow("9*9��ֵ�˲�", 0);
	imshow("9*9��ֵ�˲�", dst1);

	//��˹�˲�����/��ͨ��
	GaussianFilter(src, dst2, 9, 2);
	namedWindow("9*9��˹�˲�", 0);
	imshow("9*9��˹�˲�", dst2);

	//Laplacian
	Laplacian(src, dst4);
	namedWindow("Laplacian", 0);
	imshow("Laplacian", dst4);

	//Robert
	int x[3][3] = { 0, 0, 0, 0, -1, 0, 0, 0, 1 };
	int y[3][3] = { 0, 0, 0, 0, 0, -1, 0, 1, 0 };
	//rob_sob(src, dst5, x, y);
	namedWindow("Robert", 0);
	imshow("Robert", dst5);

	//Sobel
	int _x[3][3] = { -1, -2, -1, 0, 0, 0, -1, -2, -1};
	int _y[3][3] = { -1, 0, 1, -2, 0, 2, -1, 0, 1};
	//rob_sob(src, dst6, x, y);
	namedWindow("Sobel", 0);
	imshow("Sobel", dst6);

	//�������㷨:1,����Ĥ��>1��������<1��ǿ������ģ��Ĺ���
	int k = 2;
	//enhanceFilter(src, dst7, 3, k);
	namedWindow("Sobel", 0);
	imshow("Sobel", dst7);
	waitKey(0);
	return 0;
}




void Filter(const Mat &src, Mat &dst, int ksize, double** templateMatrix) {
	assert(src.channels() || src.channels() == 3);
	//�ڿ���̨��ӡģ��
	cout << ksize << "*" << ksize << "ģ��Ϊ�� " << endl;
	for (int i = 0; i < ksize; i++)
	{
		for (int j = 0; j < ksize; j++)
		{
			cout << templateMatrix[i][j] << " ";
		}
		cout << endl;
	}
	//ʹ��ģ�����ƽ������
	int border = ksize / 2;
	copyMakeBorder(src, dst, border, border, border, border, BorderTypes::BORDER_REFLECT);
	int channels = src.channels();
	int cols = dst.cols - border;
	int rows = dst.rows - border;
	for (int i = border; i < rows; i++) {
		for (int j = border; j < cols; j++) {
			double sum[3] = { 0 };
			for (int k = -border; k <= border; k++) {
				for (int m = -border; m <= border; m++) {
					if (channels == 1) {
						sum[0] += (double)templateMatrix[k + border][k + border] * dst.at<uchar>(i + k, j + m);
					}
					else if (channels == 3) {
						Vec3b rgb = dst.at<Vec3b>(i + k,j + m);
						auto tmp = templateMatrix[border + k][border + m];
						sum[0] += tmp*rgb[0];
						sum[1] += tmp*rgb[1];
						sum[2] += tmp*rgb[2];
					}
				}
			}
			//�޶�����ֵ��0-255֮��
			for (int i = 0; i < channels; i++) {
				if (sum[i] < 0)
					sum[i] = 0;
				else if (sum[i] > 255)
					sum[i] = 255;
			}
			//
			if (channels == 1) {
				dst.at<uchar>(i, j) = static_cast<uchar>(sum[0]);
			}
			else if (channels == 3) {
				//Vec3b rgb = { static_cast<uchar>(sum[0]), static_cast<uchar>(sum[1]), static_cast<uchar>(sum[2]) };
				Vec3b rgb;
				rgb[0] = static_cast<uchar>(sum[0]);
				rgb[1] = static_cast<uchar>(sum[1]);
				rgb[2] = static_cast<uchar>(sum[2]);

				dst.at<Vec3b>(i, j) = rgb;
			}
		}
	}
	for (int i = 0; i < ksize; i++)
		delete[] templateMatrix[i];
	delete[] templateMatrix;
}

void MeanFilter(const Mat &src, Mat &dst, int ksize) {
	//generate mean mask
	double **templateMatrix = new double *[ksize];
	for (int i = 0; i < ksize; i++) {
		templateMatrix[i] = new double[ksize];
	}
	int tmp = ksize*ksize;
	int origin = ksize / 2;
	for (int i = 0; i < ksize; i++) {
		for (int j = 0; j < ksize; j++) {
			templateMatrix[i][j] = 1.0 / tmp;
		}
	}
	Filter(src, dst, ksize, templateMatrix);
}

void GaussianFilter(const Mat &src, Mat &dst, int ksize, double sigma)
{
	//generate gaussian mask
	const static double pi = 3.1415926;
	// ���ݴ��ڴ�С��sigma���ɸ�˹�˲���ģ��
	// ����һ����ά���飬������ɵĸ�˹ģ�����
	double **templateMatrix = new double*[ksize];
	for (int i = 0; i < ksize; i++)
		templateMatrix[i] = new double[ksize];
	int origin = ksize / 2; // ��ģ�������Ϊԭ��
	double x2, y2;
	double sum = 0;
	for (int i = 0; i < ksize; i++)
	{
		x2 = pow(i - origin, 2);
		for (int j = 0; j < ksize; j++)
		{
			y2 = pow(double(j - origin), 2);
			// ��˹����ǰ�ĳ������Բ��ü��㣬���ڹ�һ���Ĺ����и���ȥ
			double g = exp(-(x2 + y2) / (2 * sigma * sigma));
			sum += g;
			templateMatrix[i][j] = g;
		}
	}

	double k = 1 / sum;
	for (int i = 0; i < ksize; i++) {
		for (int j = 0; j < ksize; j++) {
			templateMatrix[i][j] *= k;
		}
	}
	Filter(src, dst, ksize, templateMatrix);
}

void Laplacian(const Mat &src, Mat &dst) {
	vector<double> list = { -1, -1, -1, -1, 4, -1, -1, -1, -1};
	double** templateMatrix = new double*[3];
	for (int i = 0; i < 3; i++) {
		templateMatrix[i] = new double[3];
	}
	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			templateMatrix[i][j] = list[k++];
		}
	}
	Filter(src, dst, 3,templateMatrix);
}

void rob_sob(const Mat& src, Mat& dst, int x[][3], int y[][3]) {
	int border =  1;
	copyMakeBorder(src, dst, border, border, border, border, BorderTypes::BORDER_REFLECT);
	int channels = src.channels();
	int cols = dst.cols - border;
	int rows = dst.rows - border;
	for (int i = border; i < rows; i++) {
		for (int j = border; j < cols; j++) {
			int sum[3][3] = { 0 };
			for (int k = -border; k <= border; k++) {
				for (int m = -border; m <= border; m++) {
					if (channels == 1) {
						sum[0][0] += (int)x[k + border][k + border] * dst.at<uchar>(i + k, j + m);
						sum[0][1] += (int)y[k + border][k + border] * dst.at<uchar>(i + k, j + m);
					}
					else if (channels == 3) {
						Vec3b rgb = dst.at<Vec3b>(i + k, j + m);
						auto tmp1 = x[border + k][border + m];
						auto tmp2 = y[border + k][border + m];
						sum[0][0] += tmp1*rgb[0];
						sum[1][0] += tmp1*rgb[1];
						sum[2][0] += tmp1*rgb[2];
						sum[0][1] += tmp2*rgb[0];
						sum[1][1] += tmp2*rgb[1];
						sum[2][1] += tmp2*rgb[2];
					}
				}
			}
			//�޶�����ֵ��0-255֮��
			for (int i = 0; i < channels; i++) {
				sum[i][2] = sum[i][0] + sum[i][1];
				if (sum[i][2] < 0)
					sum[i][2] = 0;
				else if (sum[i][3] > 255)
					sum[i][2] = 255;
			}
			//
			if (channels == 1) {
				dst.at<uchar>(i, j) = static_cast<uchar>(sum[0][2]);
			}
			else if (channels == 3) {
				//Vec3b rgb = { static_cast<uchar>(sum[0]), static_cast<uchar>(sum[1]), static_cast<uchar>(sum[2]) };
				Vec3b rgb;
				rgb[0] = static_cast<uchar>(sum[0][2]);
				rgb[1] = static_cast<uchar>(sum[1][2]);
				rgb[2] = static_cast<uchar>(sum[2][2]);

				dst.at<Vec3b>(i, j) = rgb;
			}
		}
	}
}

void enhanceFilter(const Mat& src, Mat& dst, int ksize, double k) {
	int border = ksize/2;
	copyMakeBorder(src, dst, border, border, border, border, BorderTypes::BORDER_REFLECT);
	Mat src_mean = src.clone();
	MeanFilter(src, src_mean, ksize);
	int cols = dst.cols - border;
	int rows = dst.rows - border;
	int mask = 0;
	for (int i = border; i < rows; i++) {
		for (int j = border; j < cols; j++) {
			mask = src.at<uchar>(i, j) - src_mean.at<uchar>(i, j);
			dst.at<uchar>(i, j) =src.at<uchar>(i,j) + mask*k;
		}
	}
}
