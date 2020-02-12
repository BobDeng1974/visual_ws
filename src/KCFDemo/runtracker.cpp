#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"


using namespace std;
using namespace cv;

Rect selection;
bool selectObject = false;
Point origin;
Mat frame,img;
bool flag = false;
bool pause = false;

static void onMouse(int event, int x, int y, int, void*)
{
	if (!flag&&pause)
	{
		if (selectObject)
		{
			selection.x = MIN(x, origin.x);
			selection.y = MIN(y, origin.y);
			selection.width = std::abs(x - origin.x);
			selection.height = std::abs(y - origin.y);
			selection &= Rect(0, 0, frame.cols, frame.rows);
		}

		switch (event)
		{
		case CV_EVENT_LBUTTONDOWN:
			origin = Point(x, y);
			selection = Rect(x, y, 0, 0);
			selectObject = true;
			break;
		case CV_EVENT_LBUTTONUP:
			selectObject = false;
			if (selection.width > 0 && selection.height > 0)
				rectangle(img, selection, Scalar(0, 255, 255));
			imshow("Image", img);
			flag = true;
			break;
		}
	}
}

void Help()
{
	cout<<"������ʽ������ʱ��֤��Ƶ����Ϊ����ڣ�:"<<endl;
	cout<<"\t����p�����϶����ѡ����ʼ���ٴΰ���p������ʼ����"<<endl;
	cout<<"\t����r�������ó�ʼ��"<<endl;
	cout<<"\t����ESC�����˳�����\n\n\n\n\n";
}

int main(int argc, char* argv[]){	
	system("color 5E");
	if (argc > 5) return -1;
	Help();
	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = true;
	bool LAB = false;

	for(int i = 0; i < argc; i++){
		if ( strcmp (argv[i], "hog") == 0 )
			HOG = true;
		if ( strcmp (argv[i], "fixed_window") == 0 )
			FIXEDWINDOW = true;
		if ( strcmp (argv[i], "singlescale") == 0 )
			MULTISCALE = false;
		if ( strcmp (argv[i], "show") == 0 )
			SILENT = false;
		if ( strcmp (argv[i], "lab") == 0 ){
			LAB = true;
			HOG = true;
		}
		if ( strcmp (argv[i], "gray") == 0 )
			HOG = false;
	}
	
	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
	// Tracker results
	Rect result;
	VideoCapture cap;
	string path = "../car.mp4";
	int number;
	cout << "����������ѡ��ķ�ʽ:\n\t1.����ͷ\n\t2.��Ƶ��\n";
	cin >> number;
	switch (number)
	{
	case 1:
		cap.open(0);
		if (!cap.isOpened())
		{
			cout << "��ȡ����ͷ����" << endl;
			char c;
			cin >> c;
		}
		break;
	case 2:
		cap.open(path);
		if (!cap.isOpened())
		{
			cout << "��ȡ��Ƶ����\n" << endl;
			char c;
			cin >> c;
		}
		break;
	}	
	namedWindow("Image", 2);
	setMouseCallback("Image", onMouse, 0);
	int nFrames = 0;
	for (;;)
	{
		if (!pause)
		{
			cap >> frame;
			if (frame.empty())
				break;
			frame.copyTo(img);
			if (flag)
			{
				result = tracker.update(img);
				rectangle(img, Point(result.x, result.y), Point(result.x + result.width, result.y + result.height), Scalar(0, 255, 255), 1, 8);
				nFrames++;
			}
		}
		else
		{
			if (flag&&nFrames == 0)
			{
				tracker.init(selection, img);//Rect(xMin, yMin, width, height)
			}
		}
		imshow("Image", img);
		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
		case 'p':
			pause = !pause;
			break;
		case 'r':
			flag = false;
			nFrames = 0;
			frame.copyTo(img);
			imshow("Image", img);
			break;
		}
	}
}


