#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <opencv2\opencv.hpp>
#include <atlbase.h>

#include "FFT.h"

FFT fft;

FILE *fp, *fp2;

//表示部分
std::vector<BYTE> colorBuffer;

//表示用
#define ImageWidth 1300
#define ImageHeight 600

#define GraphWidth 1200
#define GraphHeight 200

int datastep = 0;

cv::Mat MainImageWindow(ImageHeight, ImageWidth, CV_8UC3, cv::Scalar(255, 255, 255));
std::vector<UINT16> depthBuffer;

char readfilename[] = "ECG.csv";
char writefilename[] = "RRI.csv";

//描画関数　OpenCVベース
void line(cv::Mat & Image, int x1, int y1, int x2, int y2, int R, int G, int B, int width) {
	cv::line(Image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(B, G, R), width, CV_AA);
}

void rect(cv::Mat & Image, int x1, int y1, int x2, int y2, int R, int G, int B, int width) {
	cv::rectangle(Image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(B, G, R), width, CV_AA);
}

void circle(cv::Mat & Image, int cx, int cy, int radius, int R, int G, int B, int width) {
	cv::circle(Image, cv::Point(cx, cy), radius, cv::Scalar(B, G, R), width, CV_AA);
}

void initialize()
{
	int i, j, k, l;
	bool init_HBD = false;
	const int buf_size = 300;
	char buf[buf_size];
	double data[2][buf_size], sdata[2], ddata, max_ddata, tdata[2], tddata;

	//全データ数（行数）の確認
	fp = fopen(readfilename, "r");
	if (fp) {
		while (fgets(buf, buf_size, fp) != NULL) {
			datastep++;
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}

	fp = fopen(readfilename, "r");
	fp2 = fopen(writefilename, "a");

	if (fp) {
		j = 0; k = 0;
		for (i = 0; i < datastep; i++) {
			fscanf(fp, "%lf,%lf\n", &data[0][j], &data[1][j]);
			if (i > 0) {
				sdata[0] = 0.75 * sdata[0] + 0.25 * data[1][j];
				sdata[1] = 0.9 * sdata[1] + 0.1 * data[1][j];
				ddata = sdata[0] - sdata[1];

				if (ddata > max_ddata) {
					max_ddata = ddata;
					tdata[0] = data[0][j];
				}

				if (init_HBD == false) {
					if (j == 99) {
						tdata[1] = tdata[0];
						max_ddata = 0;
					}
					else if (j == 100) {
						tddata = tdata[0] - tdata[1];
						tdata[1] = tdata[0];
						fprintf(fp2, "%lf,100,%lf\n", tdata[0], tddata);
						max_ddata = 0;
						init_HBD = true;
					}
				}
				else {
					if (data[0][j] > (1.3 * tddata + tdata[1])) {
						tddata = tdata[0] - tdata[1];
						tdata[1] = tdata[0];
						fprintf(fp2, "%lf,100,%lf\n", tdata[0], tddata);
						max_ddata = 0;
					}
				}

				j++;
				if (j == 200) j = 0;
			}
			else {
				sdata[0] = data[1][j];
				sdata[1] = data[1][j];
				max_ddata = 0;
				j++;
			}
		}
	}

	fclose(fp);
	fclose(fp2);

	//const int samples = 1024, bitSize = 10;
	//int i, j, k, l;
	//const int buf_size = 100;
	//char buf[buf_size];
	//double *fftbuf[2];
	//double *fftbuf2[2];
	//double *spbuf[2];
	//double interval = 50.0 / (double)samples, tsum = 0;

	//for (i = 0; i < 2; i++) {
	//	fftbuf[i] = (double*)malloc(sizeof(double) * samples);
	//	fftbuf2[i] = (double*)malloc(sizeof(double) * samples);
	//	spbuf[i] = (double*)malloc(sizeof(double) * samples);
	//}

	//line(MainImageWindow, 50, 50, 50, 50 + GraphHeight, 0, 0, 0, 1);
	//line(MainImageWindow, 50, 50 + GraphHeight, 50 + GraphWidth, 50 + GraphHeight, 0, 0, 0, 1);

	//line(MainImageWindow, 50, 50 + GraphHeight, 50, 50 + 2 * GraphHeight, 0, 0, 0, 1);
	//line(MainImageWindow, 50, 50 + 2 * GraphHeight, 50 + GraphWidth, 50 + 2 * GraphHeight, 0, 0, 0, 1);

	////全データ数（行数）の確認
	//fp = fopen(readfilename, "r");
	//if (fp) {
	//	while (fgets(buf, buf_size, fp) != NULL) {
	//		datastep++;
	//		memset(buf, 0, sizeof(buf));
	//	}
	//	fclose(fp);

	//	for (i = 0; i < 2; i++) data[i] = (double*)malloc(sizeof(double) * datastep); //動的メモリの確保

	//}
	//fp = fopen(readfilename, "r");
	//fp2 = fopen(writefilename, "a");


	//double plot[2], ps[2];
	//double alfa = 200.0 / 1.5, beta = 30.0;
	//double gamma[2] = { 1.0, 1.0 };
	//double rate, rate2, ave;

	//if (fp) {
	//	j = 0; k = 0;
	//	for (i = 0; i < datastep; i++) {
	//		fscanf(fp, "%lf,%lf\n", &data[0][i], &data[1][i]);
	//		//if (i < samples) { fftbuf[0][i] = (data[0][i] * 0.1)* fft.window_func(WF_Rectangular, i, samples); fftbuf[1][i] = 0; }
	//		if (i > 0) {
	//			gamma[1] = gamma[0];
	//			rate = (int)((data[1][i] / ave) + 0.5); 
	//			rate2 = (int)((data[1][i] / data[1][i-1]) + 0.3);
	//			if (rate >= 2.0)
	//				gamma[0] = 1.0 / rate; // 0.5;//0.01 / fabs(data[1][i] - data[1][i - 1]);
	//			else if (rate2 >= 2.0)
	//				gamma[0] = 1.0 / rate2; // 0.5;//0.01 / fabs(data[1][i] - data[1][i - 1]);
	//			else
	//				gamma[0] = 1.0;
	//			data[1][i] = gamma[0] * data[1][i];//(gamma[0] * (data[1][i] - data[1][i - 1]) + data[1][i - 1]);
	//			ave = 0.9 * ave + 0.1 * data[1][i];

	//			while (tsum < data[0][i]) {
	//				if (j < samples) {
	//					fftbuf[0][j] = (((data[1][i] - data[1][i - 1]) / (data[0][i] - data[0][i - 1])) * (tsum - data[0][i - 1]) + data[1][i - 1]);
	//					fftbuf[1][j] = 0.0;
	//					fftbuf2[0][j] = fftbuf[0][j] * fft.window_func(WF_Rectangular, j, samples);
	//					fftbuf2[1][j] = 0.0;

	//					j++;
	//				}
	//				else {
	//					for (l = 0; l < samples - 1; l++) {
	//						fftbuf[0][l] = fftbuf[0][l + 1];
	//						fftbuf[1][l] = 0.0;
	//						fftbuf2[0][l] = fftbuf[0][l] * fft.window_func(WF_Rectangular, l, samples);
	//						fftbuf2[1][l] = 0.0;
	//					}
	//					fftbuf[0][j - 1] = (((data[1][i] - data[1][i - 1]) / (data[0][i] - data[0][i - 1])) * (tsum - data[0][i - 1]) + data[1][i - 1]);
	//					fftbuf[1][j - 1] = 0.0;
	//					fftbuf2[0][j - 1] = fftbuf[0][j - 1] * fft.window_func(WF_Rectangular, j - 1, samples);
	//					fftbuf2[1][j - 1] = 0.0;
	//				}

	//				tsum += interval;
	//			}

	//			plot[1] = plot[0];
	//			plot[0] = alfa * data[1][i];
	//			line(MainImageWindow, 50 + (k + 1), (50 + GraphHeight) - plot[0], 50 + k, (50 + GraphHeight) - plot[1], 0, 0, 255, 1);

	//			if (j == samples) {
	//				fft.fft_cal(fftbuf2[0], fftbuf2[1], spbuf[0], spbuf[1], bitSize);

	//				double  f, LF = 0, HF = 0;

	//				for (l = 0; l < samples / 2; l++) {
	//					if (l > 20) // 
	//						break;
	//					else if (l > 0) {
	//						if (l < 8) {
	//							LF += sqrt(spbuf[0][l] * spbuf[0][l] + spbuf[1][l] * spbuf[1][l]);
	//						}
	//						else {
	//							HF += sqrt(spbuf[0][l] * spbuf[0][l] + spbuf[1][l] * spbuf[1][l]);
	//						}
	//					}

	//				}
	//				ps[1] = ps[0];
	//				ps[0] = (LF / HF) * 100.0;
	//				fprintf(fp2, "%lf,%lf,%lf\n", data[0][i], data[1][i], (LF / HF));
	//				line(MainImageWindow, 50 + (k + 1), (50 + 2 * GraphHeight) - ps[0], 50 + k, (50 + 2 * GraphHeight) - ps[1], 255, 0, 0, 1);
	//			}
	//			k++;
	//		}
	//		else {
	//			ave = data[1][i];
	//			plot[0] = data[1][i];
	//			plot[0] = alfa * (double)plot[0];
	//			tsum = data[0][i];
	//		}


	//	}
	//}

	//fclose(fp);
	//fclose(fp2);
	////double tm = 0;
	////fp = fopen("data3.csv", "a");
	////if (fp) {
	////	tm = data[0];
	////	fprintf(fp, "%lf,%lf\n", tm, data[0]);
	////	for (i = 1; i < datastep; i++) {
	////		if (data[i - 1] != data[i]) {
	////			tm += data[i];
	////			fprintf(fp, "%lf,%lf\n", tm, data[i]);
	////		}
	////	}
	////}
	////fclose(fp);



	//for (i = 0; i < 2; i++) {
	//	free(data[i]);
	//	free(fftbuf[i]);
	//	free(fftbuf2[i]);
	//	free(spbuf[i]);
	//}

}

void main()
{
	initialize();
	cv::imshow("ImageWindow", MainImageWindow);

	cv::waitKey(0);
}




