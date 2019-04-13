
#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

void MOG2Test();




void MOG2Test() {
	//create Background Subtractor objects
	//ʵ��BackgroundSubtractorMOG2�Ĺ��췽��,ʵ��ǰ�󱳾�����
	//ͨ�����Ǽٶ�û����������ľ�̬��������һЩ�������ԣ�
	//������һ��ͳ��ģ��������GMM�����ø�˹ģ�ͣ������Ƕ����˹ģ�͵ļ�Ȩ�ͻ����һ����ģ�ⱳ�������ԡ�
	//����һ����֪�������ģ�ͣ������������ͨ���������ͼ���в�������һ����ģ�͵Ĳ�������⵽��
	Ptr<BackgroundSubtractor> pBackSub;
	pBackSub = createBackgroundSubtractorMOG2();
	//pBackSub = createBackgroundSubtractorKNN();
	VideoCapture capture("D:workspace/images/test.mp4");
	if (!capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open mp4: " << endl;
		return;
	}
	Mat frame, fgMask;
	while (true) {
		capture >> frame;
		if (frame.empty())
			break;
		//update the background model
		pBackSub->apply(frame, fgMask);
		//get the frame number and write it on the current frame
		rectangle(frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
		stringstream ss;
		ss << capture.get(CAP_PROP_POS_FRAMES);
		string frameNumberString = ss.str();
		putText(frame, frameNumberString.c_str(), Point(15, 15),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
		//show the current frame and the fg masks
		imshow("Frame", frame);
		imshow("FG Mask", fgMask);
		//get the input from the keyboard
		int keyboard = waitKey(30);
		if (keyboard == 'q' || keyboard == 27)
			break;
	}
}

int main() {
	MOG2Test();
	return 0;
}
