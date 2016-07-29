#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <stdio.h>
using namespace cv;

// 实现检测出的矩形窗口的非极大值抑制
// numBoxes：窗口数目
// points：窗口左上角坐标点
// oppositePoints：窗口右下角坐标点
// score：窗口得分
// overlapThreshold：重叠阈值控制
// numBoxesOut：输出窗口数目
// pointsOut：输出窗口左上角坐标点
// oppositePoints：输出窗口右下角坐标点
// scoreOut：输出窗口得分 
int nonMaximumSuppression(int numBoxes, const CvPoint *points,
	const CvPoint *oppositePoints, const float *score,
	float overlapThreshold,
	int *numBoxesOut, CvPoint **pointsOut,
	CvPoint **oppositePointsOut, float **scoreOut)
{
	int i, j, index;
	float* box_area = (float*)malloc(numBoxes * sizeof(float));   // 定义窗口面积变量并分配空间 
	int* indices = (int*)malloc(numBoxes * sizeof(int));          // 定义窗口索引并分配空间 
	int* is_suppressed = (int*)malloc(numBoxes * sizeof(int));    // 定义是否抑制表标志并分配空间 
	// 初始化indices、is_supperssed、box_area信息 
	for (i = 0; i < numBoxes; i++)
	{
		indices[i] = i;
		is_suppressed[i] = 0;
		box_area[i] = (float)( (oppositePoints[i].x - points[i].x + 1) *
			(oppositePoints[i].y - points[i].y + 1));
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

	*numBoxesOut = 0;    // 初始化输出窗口数目0 
	for (i = 0; i < numBoxes; i++)
	{
		if (!is_suppressed[i]) (*numBoxesOut)++;    // 统计输出窗口数目 
	}

	*pointsOut = (CvPoint *)malloc((*numBoxesOut) * sizeof(CvPoint));           // 分配输出窗口左上角坐标空间 
	*oppositePointsOut = (CvPoint *)malloc((*numBoxesOut) * sizeof(CvPoint));   // 分配输出窗口右下角坐标空间 
	*scoreOut = (float *)malloc((*numBoxesOut) * sizeof(float));                // 分配输出窗口得分空间 
	index = 0;
	for (i = 0; i < numBoxes; i++)                  // 遍历所有输入窗口 
	{
		if (!is_suppressed[indices[i]])             // 将未发生抑制的窗口信息保存到输出信息中 
		{
			(*pointsOut)[index].x = points[indices[i]].x;
			(*pointsOut)[index].y = points[indices[i]].y;
			(*oppositePointsOut)[index].x = oppositePoints[indices[i]].x;
			(*oppositePointsOut)[index].y = oppositePoints[indices[i]].y;
			(*scoreOut)[index] = score[indices[i]];
			index++;
		}

	}

	free(indices);          // 释放indices空间 
	free(box_area);         // 释放box_area空间 
	free(is_suppressed);    // 释放is_suppressed空间 

	return 1;
}

// 排序函数，排序后进行交换的是indices中的数据
// n：排序总数
// x：带排序数
// indices：初始为0~n-1数目 
static void sort(int n, const float* x, int* indices)
{
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