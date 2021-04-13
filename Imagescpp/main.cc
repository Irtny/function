/*
 * main.cc
 *
 *  Created on: 6 окт. 2020 г.
 *      Author: fomin
 */




// Шаблон программы для задачи от 5.10.2020-6.10.2020
//
/*
	Задача:
	Построить график функции
		z = sinc(sqrt(x*x + y*y))
	Шаг 1. Придумать формулы для перевода координат из трёхмерной системы
			(x, y, z) в координаты точки на картинке (sx, sy)
			с учётом масштабных коэффициентов по всем трём осям
	Шаг 2. Построить график функции, перебирая одну координату с большим шагом,
			а вторую - с маленьким шагом.
	Шаг 2.5. Построить график ещё раз, поменяв местами шаг по осям.
		(на данном этапе у нас уже получится некий график, но....)
	Шаг 3. Для сокрытия невидимых линий графика реализовать алгоритм
			"плавающего горизонта"
				1. График всегда рисуется от ближних объектов к дальним
				2. Если очередная точка находится ниже линии горизонта,
					то её не рисуем
				3. Если очередная точка находится выше линии горизонта, то:
					* точку рисуем;
					* поднимаем в этой колонке изображения горизонт до уровня
					  новой точки
				4. Перед вторым проходом рисования графика (в перпендикулярных
					плоскостях) горизонт опускаем.
		Область построения: -30 <= x <= 30 и -30 <= y <= 30 (например)
 */

#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cassert>

// Это у нас структура заголовка TGA-файла
#pragma pack(push, 1)
struct TGA_Header
{
	uint8_t id_len;			// Длина идентификатора
	uint8_t pal_type;		// Тип палитры
	uint8_t img_type;		// Тип изображения
	uint8_t pal_desc[5];	// Описание палитры
	uint16_t x_pos;			// Положение по оси X
	uint16_t y_pos;			// Положение по оси Y
	uint16_t width;			// Ширина
	uint16_t height;		// Высота
	uint8_t depth;			// Глубина цвета
	uint8_t img_desc;		// Описатель изображения
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1920;
constexpr uint16_t IMG_HEIGHT = 1080;
constexpr uint32_t COL_BACKGROUND = 0xff003f3f;
constexpr uint32_t COL_FOREGROUND = 0xffcfcfcf;
constexpr double kz = 300., kx = 15., ky = 15.,
		minx = -30., miny = -30., maxx= 30., maxy = 30.,
		stepx = 0.5, stepy = 0.01;



// Тут могли бы разместиться Ваши константы

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
	// Построить график функции в некотором буфере
	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;


	// Здесь мог бы разместиться Ваш код.

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


	// Записать построенное изображение в файл формата TGA
	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;

	tga_file.open("output.tga",std::ios::out|std::ios::binary);
	// Записываем заголовок и данные картинки
	tga_file.write(reinterpret_cast<char*>(&hdr),sizeof(TGA_Header));
		tga_file.write(reinterpret_cast<char*>(&picture[0]),IMG_WIDTH*IMG_HEIGHT*4);
	// Закрываем файл
	tga_file.close();

	std::cout<<"Ok!"<<std::endl;

	return 0;
}
