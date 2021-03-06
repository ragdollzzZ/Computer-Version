#include "pch.h"
#include <iostream>
#include "ImgConverse.h"
#include <map>
#include <math.h>
#include "canny.h"
#include "canny.h"
#define PI 3.141592653589793

ImgConverse::ImgConverse() {
	img.load_jpeg("a7.jpg");
	sourceImg.load_jpeg("a8.jpg");
	targetImg.load_jpeg("a2.jpg");
	//cout << targetImg._spectrum << endl;
	for (int i = 0; i < 3; i++) {
		mean0[i] = 0.0; mean1[i] = 0.0;
		std0[i] = 0.0; std1[i] = 0.0;
	}
}
//将图片转为灰度图
void ImgConverse::toGray() {
	CImg<int> pic(img._width, img._height, 1, 1, 0);
	cimg_forXY(img, x, y) {
		pic(x, y) = img(x, y, 0) * 0.2126 + img(x, y, 1) * 0.7152 + img(x, y, 2) * 0.0722;
	}
	gray_img = pic;
}

void ImgConverse::Hist_gray() {
	if (img._spectrum == 3)
		toGray();
	CImg<int> pic = gray_img;
	pic.display("处理前图片");
	//用于获取各个灰度级对应像素点的个数
	CImg<int> hist = pic.histogram(256, 0, 255);
	//描绘直方图
	hist.display_graph("处理前直方图");
	pic = Hist(gray_img);
	pic.display("处理后图片");
	hist_equalImg = pic;
	hist = pic.histogram(256, 0, 255);
	hist.display_graph("处理后直方图");
}

void ImgConverse::Hist_impress() {
	if (img._spectrum == 3)
		toGray();
	CImg<int> pic = gray_img;
	int scale = 1, thresh_low = 40, thresh_high = 60;
	double sigma = 1.0;
	canny s;
	CImg<int> picture = s.process("a.bmp", scale, sigma, thresh_low, thresh_high);
	CImg<int> img1 = picture;
	int size = img1.size();
	//pic.resize(img1.width(), img1.height());
	img1.display();
	double sum = 0.0;
	map<int, int> index;
	int hist[256];
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}
	int ssize = 0;
	cimg_forXY(pic, x, y) {
		if (img1._atXY(x, y) != 0) {
			hist[pic(x, y)]++;
			ssize++;
		}
	}
	int max = 0;
	for (int i = 0; i < 256; i++) {
		if (hist[i] != 0 && i > max)
			max = i;
	}
	for (int i = 0; i < max; i++) {
		sum += (double)hist[i] / ssize;
		int Sum = (int)(sum * max);
		index.insert(pair<int, int>(i, Sum));
	}
	/*map<int, int>::iterator iter = index.begin();
	map<int, int>::iterator end = index.end();
	while (iter != end) {
		cout << iter->first << " " << iter->second << endl;
		iter++;
	}*/
	cimg_forXY(pic, x, y) {
		if (img1._atXY(x, y) != 0)
			pic(x, y) = index[pic(x, y)];
	}
	pic.display("直方图改进后的效果");
}

void ImgConverse::Hist_color() {
	CImg<int> _img = img;
	_img.display("处理前图片");
	CImg<int> hist = _img.histogram(256, 0, 255);
	hist.display_graph("处理前直方图");
	CImg<int> img_r(img._width, img._height, 1, 1, 0);
	CImg<int> img_g = img_r, img_b = img_r;
	cimg_forXY(img, x, y) {
		img_r(x, y) = img(x, y, 0);
		img_g(x, y) = img(x, y, 1);
		img_b(x, y) = img(x, y, 2);
	}
	img_r = Hist(img_r);
	img_g = Hist(img_g);
	img_b = Hist(img_b);
	CImg<int> pic(img._width, img._height, 1, 3, 0);
	cimg_forXY(img, x, y) {
		pic(x, y, 0) = img_r(x, y);
		pic(x, y, 1) = img_g(x, y);
		pic(x, y, 2) = img_b(x, y);
	}
	pic.display("处理后图片");
	hist_equalImg = pic;
	hist = pic.histogram(256, 0, 255);
	hist.display_graph("处理后直方图");
}

CImg<int> ImgConverse::Hist(CImg<int> picture) {
	CImg<int> img1 = picture;
	CImg<int> pic = img1;
	int size = img1.size();
	CImg<int> hist = img1.histogram(256, 0, 255); //histogram的第一个参数是共有256个灰度级，第二个参数是灰度的最小值，第三个是灰度最大值
	double sum = 0.0;
	map<int, int> index;
	for (int i = 0; i < 256; i++) {
		sum += (double)hist(i) / size;
		int Sum = (int)(sum * 255);
		index.insert(pair<int, int>(i, Sum));
	}
	map<int, int>::iterator iter = index.begin();
	map<int, int>::iterator end = index.end();
	cimg_forXY(pic, x, y) {
		pic(x, y) = index[pic(x, y)];
	}
	return pic;
}

//将图片由rgb空间转为hsi空间并对其亮度空间进行直方图均衡
void ImgConverse::RGB_HSI() {
	CImg<int> img_ = img;
	img_.display("处理前图片");
	CImg<int> img0 = img_.histogram(256, 0, 255);
	img0.display_graph("处理后直方图");
	CImg<float> pic = img; //定义float类型的目的是对其进行归一化
	CImg<float> hsi = img;
	CImg<float> hsi_i(img.width(), img.height(), 1, 1, 0);
	hsi.RGBtoHSI(); //将rgb空间转换到hsi(色彩,饱和度,亮度)空间，只对其中的亮度空间进行处理
	cimg_forXY(hsi, x, y) {
		hsi_i(x, y) = hsi(x, y, 2);
	}
	CImg<int> hsi_ii = hsi_i.normalize(0,255);
	hsi_ii = Hist(hsi_ii);
	hsi_i = hsi_ii;
	hsi_i = hsi_i.normalize(0,1);
	cimg_forXY(hsi, x, y) {
		hsi(x, y, 2) = hsi_i(x, y);
	}
	hsi.HSItoRGB();
	CImg<int> hist = hsi.normalize(0, 255);
	hist.display("处理后图片");
	hist.save("aa7.jpg");
	hist_equalImg = hist;
	CImg<int> hist0 = hist_equalImg.histogram(256, 0, 255);
	hist0.display_graph("处理后直方图");
}

CImg<float> ImgConverse::RGB_LAB(CImg<float> picture, float *mean, float *std) {
	CImg<float> pic = picture;
	float sum[3];
	float sum_std[3];
	for (int i = 0; i < 3; i++) {
		sum[i] = 0.0; sum_std[i] = 0.0;
	}
	int size = pic.width()*pic.height();
	cimg_forXY(pic, x, y) {
		float r = pic(x, y, 0);
		float g = pic(x, y, 1);
		float b = pic(x, y, 2);
		float L = 0.3811 * r + 0.5783 * g + 0.0402 * b;
		float M = 0.1967 * r + 0.7244 * g + 0.0782 * b;
		float S = 0.0241 * r + 0.1288 * g + 0.8444 * b;
		if (L == 0) L = log(1); else L = log(L);
		if (M == 0) M = log(1); else M = log(M);
		if (S == 0) S = log(1); else S = log(S);
		float l = (1.0 / sqrt(3)) * (L + M + S);
		float A = 1.0 / sqrt(6) * L + 1.0 / sqrt(6) * M - 2.0 / sqrt(6) * S;
		float B= 1.0 / sqrt(2) * L - 1.0 / sqrt(2) * M;
		pic(x, y, 0) = l; pic(x, y, 1) = A; pic(x, y, 2) = B;
		sum[0] += l;
		sum[1] += A;
		sum[2] += B;
	}
	mean[0] = sum[0] / size;
	mean[1] = sum[1] / size;
	mean[2] = sum[2] / size;
	cimg_forXY(pic, x, y) {
		sum_std[0] += pow((pic(x, y, 0) - mean[0]), 2);
		sum_std[1] += pow((pic(x, y, 1) - mean[1]), 2);
		sum_std[2] += pow((pic(x, y, 2) - mean[2]), 2);
	}
	std[0] = sqrt(sum_std[0] / size);
	std[1] = sqrt(sum_std[1] / size);
	std[2] = sqrt(sum_std[2] / size);
	return pic;
}

CImg<float> ImgConverse::LAB_RGB(CImg<float> picture) {
	CImg<float> pic = picture;
	cimg_forXY(pic, x, y) {
		float L = sqrt(3) / 3 * pic(x, y, 0) + sqrt(6) / 6 * pic(x, y, 1) + sqrt(2) / 2 * pic(x, y, 2);
		float M = sqrt(3) / 3 * pic(x, y, 0) + sqrt(6) / 6 * pic(x, y, 1) - sqrt(2) / 2 * pic(x, y, 2);
		float S = sqrt(3) / 3 * pic(x, y, 0) - sqrt(6) / 3 * pic(x, y, 1);
		L = exp(L);
		M = exp(M);
		S = exp(S);
		pic(x, y, 0) = 4.4679 * L - 3.5873 * M + 0.1193 * S;
		pic(x, y, 1) = -1.2186 * L + 2.3809 * M - 0.1624 * S;
		pic(x, y, 2) = 0.0497 * L - 0.2439 * M + 1.2045 * S;
	}
	return pic;
}

void ImgConverse::colorTransfer() {
	sourceImg.display("源图");
	targetImg.display("目标图");
	sourcelab = RGB_LAB(sourceImg, mean0, std0);
	targetlab = RGB_LAB(targetImg, mean1, std1);
	result = sourcelab;
	cimg_forXY(result, x, y) {
		result(x, y, 0) = (result(x, y, 0) - mean0[0])*std1[0] / std0[0] + mean1[0];
		result(x, y, 1) = (result(x, y, 1) - mean0[1])*std1[1] / std0[1] + mean1[1];
		result(x, y, 2) = (result(x, y, 2) - mean0[2])*std1[2] / std0[2] + mean1[2];
	}
	//targetlab = LAB_RGB(targetlab);
	result = LAB_RGB(result);
	result.display("结果图");
}	