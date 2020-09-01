#pragma once
#include "stdafx.h"



// Dll standard marco FILENAME_EXPORTS
#ifdef ASQDLLRLS_EXPORTS
#define DLLexport __declspec( dllexport )
#else
#define DLLexport __declspec( dllimport )
#endif

//def for debug breakpoint
#define debug
#if defined(debug)
#define Breakpoint(x)									\
{													    \
	cerr << #x << "'s value is: " << x << endl;         \
}													
#else
#define Breakpoint(x)
#endif

//export function
#ifndef __ASQDll_H__
#define __ASQDll_H__
extern "C"
{
	DLLexport void getASQ_all(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width, double**& rf_data, double**& mask_data, int rf_height, int rf_width);
	DLLexport void getASQ(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width);
	DLLexport double getHomo_Index();
	DLLexport double getCM2_ratio();
	DLLexport double getFD_ratio();
	DLLexport double** getCM2();
	DLLexport void setFanPara();
	DLLexport BYTE** getFanPtr(BYTE* pBufB);
	//DLLexport BYTE getElm(int row,int column);


}
#endif // !__ASQDll_H__

#define pi 3.14159267


/* datatype MatrixIN*/
template <class T>
class MatrixIn
{
private:
	int		row;
	int		column;
	int		element;
	T**		data;
	T***    threeData;
	bool    tag_3D=false;
	void create()
	{
		data = new T*[row];
		data[0] = new T[row*column];
		if (row > 1)
		{
			for (int i = 1; i < row; i++)
			{
				data[i] = data[i - 1] + column;
			}

		}
		return;
	}

	void create3D()
	{
		
		tag_3D = true;
		threeData = new T**[row];
		threeData[0] = new T*[row*column];
		
		for (int i = 1; i < row; i++)
		{
			threeData[i] = threeData[i - 1] + column
		}
		
		for (int i = 0; i < row; i++)
			for (int j = 0; j < column; j++)
			{
				threeData[i][j] = new T[element];
			}
		


		
	}
	
public:

	// constructor 

	MatrixIn(int x = 2, int y = 2)
	{
		row = x;
		column = y;
		create();
		return;
	}
	MatrixIn(int x, int y, int z)
	{
		row = x;
		column = y;
		element = z;
		create3D();
		return;
	}
	// create form old one 
	MatrixIn(MatrixIn<T>& oldMat);

	MatrixIn(T**& indata, int x, int y);

	MatrixIn(T*& indata, int x, int y);

	//sect 
	MatrixIn(MatrixIn<T>& old, int row, int sect_row, int column, int sect_column);

	//initial to 1D
	MatrixIn(MatrixIn<T>& old, char symbol, T thr);

	//imersize (bilinear/bicubic)
	MatrixIn(MatrixIn<T>& small, int row, int column, char size_Method);


	//destructor
	~MatrixIn() 
	{ 
		
		if (tag_3D)
		{
			del3D();
		}
		else
		{
			del();

		}
	}
	//reset 
	void setnew(int row, int column, T**& old)
	{
		del();
		this->row = row;
		this->column = column;
		create();
		givevalue(old);

	}
	//giving value
	void givevalue(T**& old)
	{
		for (int i = 0; i < row; i++)
			for (int j = 0; j < column; j++)
				data[i][j] = old[i][j];
	}

	// access member
	inline T** GetPtr()
	{
		return data;
	}
	inline T* Get1dPtr()
	{
		return data[0];
	}
	void del()
	{
		delete[] data[0];
		delete[] data;
	}
	void del3D()
	{
		tag_3D = false;
		for (int i = 0; i < row; i++)
			for (int j = 0; j < column; j++)
			{
				delete[] threeData[i][j];
			}
		delete[] threeData[0];
		delete[] threeData;
		
	}

	inline int getrow()
	{
		return row;
	}

	inline int getcolumn()
	{
		return column;
	}

	// calculation
	T getMax();

	T getMin();


	T getsum();

	T getavg();

	T getstd();

	void Abs();

	void iniData(T thr);

	void outputMatrix();

	// pointer change calculation
	int trim_count(char symbol, T thr);
	// rotate for fan shape expansion (-90 degree)
	void rotate(MatrixIn<T>& indata);

};
/* ASQ main*/ 
class  ASQ
{
private:
	const double		MHz = pow(10, 6);
	const double		us = pow(10, -6);
	const double		mm = pow(10, -3);
	const double		cm = pow(10, -2);
	double				fc;						//center frequency
	double				fs;						//sampling rate
	int					v;						//velocity
	double				pulselength;			//pulse length
	int					thr;					//threshold to reject envelope signal
	double				window_size;			//window side length (x pulse length for parametric imaging)
	double				WOR;					//window overlap ratio
	double				roi_width;				//image width
	MatrixIn<double>	indata;					//data for calculaion 
	MatrixIn<double>	mask;					//mask for calculation

	long double			getHcW(MatrixIn<double>& indata, double dy);
	void				ASQcalculation(MatrixIn<double>& indata, MatrixIn<double>& mask);

public:
	/*---------result------------------*/
	double				homo_index;
	double				FD_ratio;
	double				CM2_ratio;
	double**			CM2_data;
	int					CM2data_height;
	int					CM2data_width;

	ASQ() {}
	// for use (Matrix data "rf_data"/"mask_data" need input)
	void getASQ_all(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width, double**& rf_data, double**& mask_data, int rf_height, int rf_width)
	{
		this->fc = double(fc)*double(MHz);
		this->fs = double(fs)*double(MHz);
		this->v = velocity;
		this->pulselength = double(pulselength)*us*(this->fs);
		this->window_size = windowsize;
		this->WOR = overlap_rate;
		this->roi_width = roi_width*cm;

		indata.setnew(rf_height, rf_width, rf_data);
		mask.setnew(rf_height, rf_width, mask_data);
		ASQcalculation(indata, mask);

		return;
	}
	// for test (Matrix data load from file "rf_data.txt"/"mask.txt"
	void getASQ(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width)
	{
		this->fc = double(fc)*double(MHz);
		this->fs = double(fs)*double(MHz);
		this->v = velocity;
		this->pulselength = double(pulselength)*us*(this->fs);
		this->window_size = windowsize;
		this->WOR = overlap_rate;
		this->roi_width = roi_width*cm;

		loadfile();
		ASQcalculation(indata, mask);
	}
	void loadfile();
	
	~ASQ(){}
};

template<class T>
class Fanout
{
	
public:

	T**					fanImg;

	Fanout() {}
	
	~Fanout()
	{
			delete[] temp_X[0];
			delete[] temp_X;
			delete[] temp_Y[0];
			delete[] temp_Y;
			delete[] fanImg[0];
			delete[] fanImg;

		
	}

	void setNewpara(int data_width, int data_height, double fanangle, double img_lenX, double img_lenY);
	
	void FanImgPara();

	void calFanImg(T* pBufU);

	void canFanImg_color(T* bmpdata);

private:
	
	double**					temp_X;
	double**					temp_Y;

	const int					data_width = 512;		//pixel_x
	const int					data_height = 128;		//pixel_y

	int					fanSizeX;				//fan matrix 
	int					fanSizeY;				//fan matrix
	
	double				fanangle=60;
	double				img_length_X = 60;		//physical probe width
	double				img_length_Y = 120;		//physical detect deepth
	
	
	
	
};


