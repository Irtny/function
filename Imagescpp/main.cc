/*
 * main.cc
 *
 *  Created on: 6 ���. 2020 �.
 *      Author: fomin
 */




// ������ ��������� ��� ������ �� 5.10.2020-6.10.2020
//
/*
	������:
	��������� ������ �������
		z = sinc(sqrt(x*x + y*y))
	��� 1. ��������� ������� ��� �������� ��������� �� ��������� �������
			(x, y, z) � ���������� ����� �� �������� (sx, sy)
			� ������ ���������� ������������� �� ���� ��� ����
	��� 2. ��������� ������ �������, ��������� ���� ���������� � ������� �����,
			� ������ - � ��������� �����.
	��� 2.5. ��������� ������ ��� ���, ������� ������� ��� �� ����.
		(�� ������ ����� � ��� ��� ��������� ����� ������, ��....)
	��� 3. ��� �������� ��������� ����� ������� ����������� ��������
			"���������� ���������"
				1. ������ ������ �������� �� ������� �������� � �������
				2. ���� ��������� ����� ��������� ���� ����� ���������,
					�� � �� ������
				3. ���� ��������� ����� ��������� ���� ����� ���������, ��:
					* ����� ������;
					* ��������� � ���� ������� ����������� �������� �� ������
					  ����� �����
				4. ����� ������ �������� ��������� ������� (� ����������������
					����������) �������� ��������.
		������� ����������: -30 <= x <= 30 � -30 <= y <= 30 (��������)
 */

#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cassert>

// ��� � ��� ��������� ��������� TGA-�����
#pragma pack(push, 1)
struct TGA_Header
{
	uint8_t id_len;			// ����� ��������������
	uint8_t pal_type;		// ��� �������
	uint8_t img_type;		// ��� �����������
	uint8_t pal_desc[5];	// �������� �������
	uint16_t x_pos;			// ��������� �� ��� X
	uint16_t y_pos;			// ��������� �� ��� Y
	uint16_t width;			// ������
	uint16_t height;		// ������
	uint8_t depth;			// ������� �����
	uint8_t img_desc;		// ��������� �����������
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1920;
constexpr uint16_t IMG_HEIGHT = 1080;
constexpr uint32_t COL_BACKGROUND = 0xff003f3f;
constexpr uint32_t COL_FOREGROUND = 0xffcfcfcf;
constexpr double kz = 300., kx = 15., ky = 15.,
		minx = -30., miny = -30., maxx= 30., maxy = 30.,
		stepx = 0.5, stepy = 0.01;



// ��� ����� �� ������������ ���� ���������

double sinc(double x)
{
	if (x == 0) return 1.;
	return sin(x) / x;
}

double my_evil_function(double x, double y)
{
	return sinc(hypot(x, y));
}

double reformx(double stepx, double stepz){
	double sx=stepx*cos(30)+stepz*cos(30);
	return sx;
}

double reformy(double stepy, double stepz){
	double sy=stepy*sin(-60)+stepz*sin(-60);
	return sy;
}

void pic(std::vector<uint32_t>& pix, int x, int y, uint32_t c, std::vector<int>& h)
{
	if (x < 0 or x >= IMG_WIDTH)
		return;
	if (y > h[x])
		return;
	h[x]=y;
	if(y < 0)
		return;
	assert(y * IMG_WIDTH + x >= 0);
	assert(y * IMG_WIDTH + x < IMG_WIDTH * IMG_HEIGHT);
	pix[y*IMG_WIDTH + x] = c;

}

int main()
{
	// ��������� ������ ������� � ��������� ������
	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;


	// ����� ��� �� ������������ ��� ���.

	int sx=0,sy=0;
	double z;
	std::fstream tga_file;
	std::vector<int> h_w;
	h_w.resize(IMG_WIDTH);

	for(auto && a:h_w)
		a=IMG_HEIGHT;

	//z = sinc(sqrt(x*x + y*y))
	for (double i = maxx; i >= minx; i -= stepx) {
		for (double j = maxy; j >= miny; j -= stepy) {
			z = my_evil_function(i, j);
			sx = int(IMG_WIDTH / 2 - kx * i * cos(M_PI / 6) + ky * j * cos(M_PI / 6));
			sy = int(IMG_HEIGHT / 2 + kx * i * sin(M_PI / 6) + ky * j * sin(M_PI / 6) - kz * z);
			pic(picture, sx, sy, COL_FOREGROUND,h_w);
		}
	}

	for (auto&& a : h_w)
		a = IMG_HEIGHT;


	// �������� ����������� ����������� � ���� ������� TGA
	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;

	tga_file.open("output.tga",std::ios::out|std::ios::binary);
	// ���������� ��������� � ������ ��������
	tga_file.write(reinterpret_cast<char*>(&hdr),sizeof(TGA_Header));
		tga_file.write(reinterpret_cast<char*>(&picture[0]),IMG_WIDTH*IMG_HEIGHT*4);
	// ��������� ����
	tga_file.close();

	std::cout<<"Ok!"<<std::endl;

	return 0;
}
