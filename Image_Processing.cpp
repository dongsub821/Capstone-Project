#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory.h>
#include <iostream>
#include <vector>
#include <time.h>


using namespace cv;
using namespace std;

typedef struct PIXEL {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char gray;
} PIXEL;

typedef struct pt {
	int x;
	int y;
} location;



PIXEL **Maps;
uint8_t **ptr;
vector<location> corner;


void IppHarrisCorner(int w,int h, double th);
void mallocMaps(int width, int height);
void mallocByte(int width, int height);
void mallocFloat(float **tmp, int width, int height);
void assignPtr(int width, int height);
void transImgToArray(IplImage* img);
void grayscaling(int width, int height);
void binarization(int width, int height);
void setRedPoint(IplImage* img);

int main() {

	Mat img[4];
	img[0] = imread("C:\\5.jpg", IMREAD_COLOR);
	img[1] = imread("C:\\2.jpg", IMREAD_COLOR);
	img[2] = imread("C:\\3.jpg", IMREAD_COLOR);
	img[3] = imread("C:\\4.jpg", IMREAD_COLOR);

	if (!img[0].data || !img[1].data || !img[2].data || !img[3].data) {
		cout << "Image File Open Failed" << endl;
		return -1;
	}
	IplImage *trans[4];
	trans[0] = &IplImage(img[0]);
	trans[1] = &IplImage(img[1]);
	trans[2] = &IplImage(img[2]);
	trans[3] = &IplImage(img[3]);

	char imgName[4] = { '1', '2', '3', '4' };
	clock_t begin, end;
	begin = clock();
	for (int i = 0; i < 1; i++) {
		int width = trans[i]->width;
		int height = trans[i]->height;
		int threshold = 100;

		mallocMaps(width, height);
		transImgToArray(trans[i]);
		grayscaling(width, height);
		//binarization(width, height);

		IppHarrisCorner(width, height, threshold);		
		setRedPoint(trans[i]);
		//cvResizeWindow("test.jpg", 320, 240);
		
		cvShowImage("test.jpg", trans[i]);

		waitKey(0);

	}
	end = clock();
	cout << ((end - begin)) << "ms" << endl;
	return 0;
}
void IppHarrisCorner(int width,int height, double th)
{
	register int i, j, x, y;

	int w = width;
	int h = height;
	
	mallocByte(w, h);
	assignPtr(w, h);
	//-------------------------------------------------------------------------
	// 1. (fx)*(fx), (fx)*(fy), (fy)*(fy) 계산
	//-------------------------------------------------------------------------

	float **dx2, **dy2, **dxy;

	dx2 = new float*[height];
	dy2 = new float*[height];
	dxy = new float*[height];

	for (int i = 0; i < height; i++) {
		dx2[i] = new float[width];
		dy2[i] = new float[width];
		dxy[i] = new float[width];
	}

	float tx, ty;
	for (j = 0; j < h ; j++)
		for (i = 0; i < w ; i++)
		{
			if (j == 0 || i == 0 || j == h - 1 || i == w - 1) {
				dx2[j][i] = 0;
				dy2[j][i] = 0;
				dxy[j][i] = 0;
				continue;
			}

			tx = (ptr[j - 1][i + 1] + ptr[j][i + 1] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j][i - 1] - ptr[j + 1][i - 1]) / 6.f;
			ty = (ptr[j + 1][i - 1] + ptr[j + 1][i] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j - 1][i] - ptr[j - 1][i + 1]) / 6.f;			
			
				dx2[j][i] = tx * tx;
				dy2[j][i] = ty * ty;
				dxy[j][i] = tx * ty;
			
		}

	//-------------------------------------------------------------------------
	// 2. 가우시안 필터링
	//-------------------------------------------------------------------------
		

	float **gdx2, **gdy2, **gdxy;
	gdx2 = new float*[height];
	gdy2 = new float*[height];
	gdxy = new float*[height];

	for (int i = 0; i < height; i++) {
		gdx2[i] = new float[width];
		gdy2[i] = new float[width];
		gdxy[i] = new float[width];
	}

	float g[5][5] = { { 1, 4, 6, 4, 1 },{ 4, 16, 24, 16, 4 },
	{ 6, 24, 36, 24, 6 },{ 4, 16, 24, 16, 4 },{ 1, 4, 6, 4, 1 } };

	for (y = 0; y < 5; y++)
		for (x = 0; x < 5; x++)
		{
			g[y][x] /= 256.f;
		}

	float tx2, ty2, txy;
	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			tx2 = ty2 = txy = 0;
			for (y = 0; y < 5; y++)
				for (x = 0; x < 5; x++)
				{					
						tx2 += (dx2[j + y - 2][i + x - 2] * g[y][x]);
						ty2 += (dy2[j + y - 2][i + x - 2] * g[y][x]);
						txy += (dxy[j + y - 2][i + x - 2] * g[y][x]);
					
				}

			gdx2[j][i] = tx2;
			gdy2[j][i] = ty2;
			gdxy[j][i] = txy;
		}

	//-------------------------------------------------------------------------
	// 3. 코너 응답 함수 생성
	//-------------------------------------------------------------------------
	
	float **crf;
	crf = new float*[height];

	for (int i = 0; i < height; i++) {
		crf[i] = new float[width];		
	}

	float k = 0.04f;
	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			crf[j][i] = (gdx2[j][i] * gdy2[j][i] - gdxy[j][i] * gdxy[j][i])
				- k * (gdx2[j][i] + gdy2[j][i])*(gdx2[j][i] + gdy2[j][i]);
		}

	//-------------------------------------------------------------------------
	// 4. 임계값보다 큰 국지적 최댓값을 찾아 코너 포인트로 결정
	//-------------------------------------------------------------------------

	corner.clear();

	float cvf_value;

	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			cvf_value = crf[j][i];
			/*
			if (cvf_value != 0) {
				printf(" hit \n");
			}
			*/
			if (cvf_value > th)
			{
				if (cvf_value > crf[j - 1][i] && cvf_value > crf[j - 1][i + 1] &&
					cvf_value > crf[j][i + 1] && cvf_value > crf[j + 1][i + 1] &&
					cvf_value > crf[j + 1][i] && cvf_value > crf[j + 1][i - 1] &&
					cvf_value > crf[j][i - 1] && cvf_value > crf[j - 1][i - 1])
				{
					location l;
					l.x = i;
					l.y = j;
					corner.push_back(l);
				}
			}
		}
}


void mallocMaps(int width, int height) { // malloc Maps array (size : width * height)
	Maps = new PIXEL*[height];	
	for (int i = 0; i < height; i++) {
		Maps[i] = new PIXEL[width];		
	}
}

void mallocByte(int width, int height) { // malloc Maps array (size : width * height)
	ptr = new uint8_t*[height];
	for (int i = 0; i < height; i++) {
		ptr[i] = new uint8_t[width];
	}
}

void assignPtr(int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			ptr[i][j] = Maps[i][j].gray;
		}
	}
}

void mallocFloat(float **tmp, int width, int height) {
	tmp = new float*[height];
	for (int i = 0; i < height; i++) {
		tmp[i] = new float[width];
	}
}


void transImgToArray(IplImage* img) { // take image's info to Maps array
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			Maps[i][j].red = img->imageData[i*img->widthStep + j * img->nChannels + 2];
			Maps[i][j].green = img->imageData[i*img->widthStep + j * img->nChannels + 1];
			Maps[i][j].blue = img->imageData[i*img->widthStep + j * img->nChannels + 0];
			Maps[i][j].gray = (Maps[i][j].red + Maps[i][j].green + Maps[i][j].blue) / 3;
		}
	}
}

void setRedPoint(IplImage* img) {
	int dx, dy;
	for (int i = 0; i < corner.size(); i++) {
		for (dx = -2; dx < 2; dx++) {
			for (dy = -2; dy < 2; dy++) {
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx)* img->nChannels + 2] = 255; // red
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx) * img->nChannels + 1] = 0; // green
				img->imageData[(corner[i].y + dy) * img->widthStep + (corner[i].x + dx)* img->nChannels + 0] = 0; // blue
			}
		}
	}
}


void grayscaling(int width, int height) { // image processing (to grayscale)
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Maps[i][j].red = Maps[i][j].gray;
			Maps[i][j].green = Maps[i][j].gray;
			Maps[i][j].blue = Maps[i][j].gray;
		}
	}
}
void binarization(int width, int height) { // image processing (to binary)
	int threshold = 127;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (Maps[i][j].gray > threshold) {
				Maps[i][j].gray = 255;
				Maps[i][j].red = 255;
				Maps[i][j].green = 255;
				Maps[i][j].blue = 255;
			}
			else {
				Maps[i][j].gray = 0;
				Maps[i][j].red = 0;
				Maps[i][j].green = 0;
				Maps[i][j].blue = 0;
			}
		}
	}
}

