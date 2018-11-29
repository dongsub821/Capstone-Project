#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory.h>
#include <iostream>
#include <vector>
#include <algorithm>
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

typedef struct {

	int group_num;
	int size;
	pt avg_location; 
	pt end_point[4]; // 0 상 1 동 2 하 3 좌

}keypoint_group;

PIXEL **Maps;
uint8_t **ptr;
vector<location> corner;
vector<int> group_index;
vector<keypoint_group> keypg;

void IppHarrisCorner(int w, int h, double th);
void mallocMaps(int width, int height);
void mallocByte(int width, int height);
void assignPtr(int width, int height);
void transImgToArray(IplImage* img);
void grayscaling(int width, int height);
void binarization(int width, int height);
void setRedPoint(IplImage* img);
int find(int a, vector<int> & v);
void join(int a, int b, vector<int> & vt);
void grouping();
void decision(vector<int> & vt, int n1, int n2);
int abs(int a);
void alloc_keyPoint();
void push_keyPoint(int n);
void setRedBox(IplImage* img, int height, int width);
bool compare(const keypoint_group &a, const keypoint_group &b) {
	return a.avg_location.x < b.avg_location.x;
}

int main() {

	Mat img[4];
	img[0] = imread("C:\\9.jpg", IMREAD_COLOR);
	img[1] = imread("C:\\10.jpg", IMREAD_COLOR);
	img[2] = imread("C:\\15.jpg", IMREAD_COLOR);
	img[3] = imread("C:\\16.jpg", IMREAD_COLOR);

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
	for (int i = 0; i < 4; i++) {
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
		grouping();
		alloc_keyPoint();
			   	

		setRedBox(trans[i],height,width);
		group_index.clear();
		keypg.clear();
		cvShowImage("test.jpg", trans[i]);

		waitKey(0);

	}
	end = clock();
	cout << ((end - begin)) << "ms" << endl;
	return 0;
}
void IppHarrisCorner(int width, int height, double th)
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
	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
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
					group_index.push_back(corner.size());
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

void setRedBox(IplImage* img, int height, int width) {
	for (int i = 0; i < keypg.size(); i++) {
		for (int YStep = 0; YStep < height; YStep++) {
			for (int XStep = 0; XStep < width; XStep++) {
				if ((XStep > keypg[i].end_point[3].x - 2 && XStep < keypg[i].end_point[3].x) || (XStep > keypg[i].end_point[1].x && XStep < keypg[i].end_point[1].x + 2)) {
					if (YStep > keypg[i].end_point[0].y - 2 && YStep < keypg[i].end_point[2].y + 2) {
						img->imageData[(YStep) * img->widthStep + (XStep)* img->nChannels + 2] = 255; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 1] = 0; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 0] = 0; // red
					}
				}
				if ((YStep > keypg[i].end_point[0].y - 2 && YStep < keypg[i].end_point[0].y) || (YStep > keypg[i].end_point[2].y && YStep < keypg[i].end_point[2].y + 2)) {
					if (XStep > keypg[i].end_point[3].x - 2 && XStep < keypg[i].end_point[1].x + 2) {
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 2] = 255; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 1] = 0; // red
						img->imageData[(YStep)* img->widthStep + (XStep)* img->nChannels + 0] = 0; // red
					}
				}
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

int find(int a, vector<int> & v) {
	if (v[a] == a) {
		return a;
	}
	else {
		v[a] = find(v[a], v);
		return v[a];
	}
}

void join(int a, int b, vector<int> & vt) {
	int aroot = find(a, vt);
	int broot = find(b, vt);
	vt[aroot] = broot;
}

void grouping() {
	for (int i = 1; i < corner.size(); i++) {
		for (int j = 0; j < i; j++) {
			decision(group_index, i, j);
		}
	}
}

void decision(vector<int> & vt, int n1, int n2) {
	
	int tx = abs(corner[n1].x - corner[n2].x);
	int ty = abs(corner[n1].y - corner[n2].y);

	int distance = (tx * tx) + (ty * ty);
	
	if (distance < 500) {
		join(n1, n2, vt);
	}	
}

int abs(int a) {
	if (a < 0)
		return (-a);
	else
		return a;
}

void alloc_keyPoint() {	

	int size = group_index.size();
	int flag;

	push_keyPoint(0);

	for (int i = 1; i < size; i++) {
		flag = 0;
		for (int j = 0; j < keypg.size(); j++) {
			if (keypg[j].group_num == group_index[i]) {
				// 해당 keypg.size  ++
				keypg[j].size++;
				keypg[j].avg_location.x += corner[i].x;
				keypg[j].avg_location.y += corner[i].y;

				//비교 0 하 우 
				if (corner[i].y < keypg[j].end_point[0].y) {
					keypg[j].end_point[0].x = corner[i].x;
					keypg[j].end_point[0].y = corner[i].y;

				}

				if (corner[i].x > keypg[j].end_point[1].x) {
					keypg[j].end_point[1].x = corner[i].x;
					keypg[j].end_point[1].y = corner[i].y;

				}

				if (corner[i].y > keypg[j].end_point[2].y) {
					keypg[j].end_point[2].x = corner[i].x;
					keypg[j].end_point[2].y = corner[i].y;

				}

				if (corner[i].x < keypg[j].end_point[3].x) {
					keypg[j].end_point[3].x = corner[i].x;
					keypg[j].end_point[3].y = corner[i].y;
				}		
				flag = 1;
				break;
			}			

		}
		if (flag == 0) {
			push_keyPoint(i);
		}
		
	}
	
	for (int i = 0; i < keypg.size(); i++) {
		keypg[i].avg_location.x /= keypg[i].size;
		keypg[i].avg_location.y /= keypg[i].size;
	}


}
void push_keyPoint(int n) {
	keypoint_group tmp;
	tmp.group_num = group_index[n];
	tmp.size = 1;
	tmp.avg_location.x = corner[n].x;
	tmp.avg_location.y = corner[n].y;	
	
	tmp.end_point[0].x = corner[n].x;
	tmp.end_point[1].x = corner[n].x;
	tmp.end_point[2].x = corner[n].x;
	tmp.end_point[3].x = corner[n].x;
	tmp.end_point[0].y = corner[n].y;
	tmp.end_point[1].y = corner[n].y;
	tmp.end_point[2].y = corner[n].y;
	tmp.end_point[3].y = corner[n].y;
	


	

	keypg.push_back(tmp);
}
