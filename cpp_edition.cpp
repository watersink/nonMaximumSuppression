#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;



static void sort(int n, const vector<float> x, vector<int> indices)
{
// 排序函数，排序后进行交换的是indices中的数据
// n：排序总数// x：带排序数// indices：初始为0~n-1数目 
	
	int i, j;
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++)
		{
			if (x[indices[j]] > x[indices[i]])
			{
				//float x_tmp = x[i];
				int index_tmp = indices[i];
				//x[i] = x[j];
				indices[i] = indices[j];
				//x[j] = x_tmp;
				indices[j] = index_tmp;
			}
		}
}



int nonMaximumSuppression(int numBoxes, const vector<CvPoint> points,const vector<CvPoint> oppositePoints, 
	const vector<float> score,	float overlapThreshold,int& numBoxesOut, vector<CvPoint>& pointsOut,
	vector<CvPoint>& oppositePointsOut, vector<float> scoreOut) 
{
// 实现检测出的矩形窗口的非极大值抑制nms
// numBoxes：窗口数目// points：窗口左上角坐标点// oppositePoints：窗口右下角坐标点// score：窗口得分
// overlapThreshold：重叠阈值控制// numBoxesOut：输出窗口数目// pointsOut：输出窗口左上角坐标点
// oppositePoints：输出窗口右下角坐标点// scoreOut：输出窗口得分
	int i, j, index;
	vector<float> box_area(numBoxes);				// 定义窗口面积变量并分配空间 
	vector<int> indices(numBoxes);					// 定义窗口索引并分配空间 
	vector<int> is_suppressed(numBoxes);			// 定义是否抑制表标志并分配空间 
	// 初始化indices、is_supperssed、box_area信息 
	for (i = 0; i < numBoxes; i++)
	{
		indices[i] = i;
		is_suppressed[i] = 0;
		box_area[i] = (float)( (oppositePoints[i].x - points[i].x + 1) *(oppositePoints[i].y - points[i].y + 1));
	}
	// 对输入窗口按照分数比值进行排序，排序后的编号放在indices中 
	sort(numBoxes, score, indices);
	for (i = 0; i < numBoxes; i++)                // 循环所有窗口 
	{
		if (!is_suppressed[indices[i]])           // 判断窗口是否被抑制 
		{
			for (j = i + 1; j < numBoxes; j++)    // 循环当前窗口之后的窗口 
			{
				if (!is_suppressed[indices[j]])   // 判断窗口是否被抑制 
				{
					int x1max = max(points[indices[i]].x, points[indices[j]].x);                     // 求两个窗口左上角x坐标最大值 
					int x2min = min(oppositePoints[indices[i]].x, oppositePoints[indices[j]].x);     // 求两个窗口右下角x坐标最小值 
					int y1max = max(points[indices[i]].y, points[indices[j]].y);                     // 求两个窗口左上角y坐标最大值 
					int y2min = min(oppositePoints[indices[i]].y, oppositePoints[indices[j]].y);     // 求两个窗口右下角y坐标最小值 
					int overlapWidth = x2min - x1max + 1;            // 计算两矩形重叠的宽度 
					int overlapHeight = y2min - y1max + 1;           // 计算两矩形重叠的高度 
					if (overlapWidth > 0 && overlapHeight > 0)
					{
						float overlapPart = (overlapWidth * overlapHeight) / box_area[indices[j]];    // 计算重叠的比率 
						if (overlapPart > overlapThreshold)          // 判断重叠比率是否超过重叠阈值 
						{
							is_suppressed[indices[j]] = 1;           // 将窗口j标记为抑制 
						}
					}
				}
			}
		}
	}

	numBoxesOut = 0;    // 初始化输出窗口数目0 
	for (i = 0; i < numBoxes; i++)
	{
		if (!is_suppressed[i]) numBoxesOut++;    // 统计输出窗口数目 
	}
	index = 0;
	for (i = 0; i < numBoxes; i++)                  // 遍历所有输入窗口 
	{
		if (!is_suppressed[indices[i]])             // 将未发生抑制的窗口信息保存到输出信息中 
		{
			pointsOut.push_back(Point(points[indices[i]].x,points[indices[i]].y));
			oppositePointsOut.push_back(Point(oppositePoints[indices[i]].x,oppositePoints[indices[i]].y));
			scoreOut.push_back(score[indices[i]]);
			index++;
		}

	}

	return true;
}

int main()
{
	Mat image=Mat::zeros(600,600,CV_8UC3);
	int numBoxes=4;
	vector<CvPoint> points(numBoxes);
	vector<CvPoint> oppositePoints(numBoxes);
	vector<float> score(numBoxes);

	points[0]=Point(200,200);oppositePoints[0]=Point(400,400);score[0]=0.99;
	points[1]=Point(220,220);oppositePoints[1]=Point(420,420);score[1]=0.9;
	points[2]=Point(100,100);oppositePoints[2]=Point(150,150);score[2]=0.82;
	points[3]=Point(200,240);oppositePoints[3]=Point(400,440);score[3]=0.5;
	
	
	float overlapThreshold=0.8;
	int numBoxesOut;
	vector<CvPoint> pointsOut;
	vector<CvPoint> oppositePointsOut;
	vector<float> scoreOut;

	nonMaximumSuppression( numBoxes,points,oppositePoints,score,overlapThreshold,numBoxesOut,pointsOut,oppositePointsOut,scoreOut);
	for (int i=0;i<numBoxes;i++)
	{
		rectangle(image,points[i],oppositePoints[i],Scalar(0,255,255),6);
		char text[20];
		sprintf(text,"%f",score[i]);
		putText(image,text,points[i],CV_FONT_HERSHEY_COMPLEX, 1,Scalar(0,255,255));
	}
	for (int i=0;i<numBoxesOut;i++)
	{
		rectangle(image,pointsOut[i],oppositePointsOut[i],Scalar(0,0,255),2);
	}
	
	imshow("result",image);

	waitKey();
	return 0;
}