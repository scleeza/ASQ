// ASQdllrls.cpp : 定義 DLL 應用程式的匯出函式。
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <ctime>
#include "ASQdllrls.h"

using namespace std;
//-----------------------------------------
//------------Mat definition-------------
//-----------------------------------------
template<class T>
MatrixIn<T>::MatrixIn(MatrixIn<T>& oldMat)
{
	row = oldMat.getrow();
	column = oldMat.getcolumn();
	create();
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			data[i][j] = oldMat.data[i][j];
	return;
}
template<class T>
MatrixIn<T>::MatrixIn(T**& indata, int x, int y)
{
	row = x;
	column = y;
	create();
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			data[i][j] = indata[i][j];
	return;
}
template<class T>
MatrixIn<T>::MatrixIn(T*& indata, int x, int y)
{ 
	row = x;
	column = y;
	create();
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			data[i][j] = indata[i*column + j];
		}
	}
}
template<class T>
MatrixIn<T>::MatrixIn(MatrixIn<T>& old, int row, int sect_row, int column, int sect_column)
{
	this->row = sect_row + 1;
	this->column = sect_column + 1;
	create();
	int row_num = 0;
	for (int count1 = row; count1 <= row + sect_row; count1++)
	{
		int column_num = 0;
		for (int count2 = column; count2 <= column + sect_column; count2++)
		{

			data[row_num][column_num] = old.GetPtr()[count1][count2];
			column_num++;

		}

		row_num++;
	}

}
template<class T>
MatrixIn<T>::MatrixIn(MatrixIn<T>& old, char symbol, T thr)
{
	int number_cancel = old.trim_count(symbol, thr);
	row = 1;
	column = (old.getrow()*old.getcolumn() - number_cancel);
	create();
	if (symbol == '<')
	{
		int column_spcecial = 0;
		for (int count1 = 0; count1 <old.getrow()*old.getcolumn(); count1++)
		{

			if (*(old.Get1dPtr() + count1)< thr)
			{
				column_spcecial -= 1;
			}
			else
			{
				*(data[0] + column_spcecial) = *(old.Get1dPtr() + count1);

			}
			column_spcecial++;

		}
	}
	else if (symbol == '>')
	{
		int column_spcecial = 0;
		for (int count1 = 0; count1 <old.getrow()*old.getcolumn(); count1++)
		{


			if (*(old.Get1dPtr() + count1)> thr)
			{
				column_spcecial -= 1;
			}
			else
			{
				*(data[0] + column_spcecial) = *(old.Get1dPtr() + count1);
			}
			column_spcecial++;

		}
	}
	else if (symbol == '=')
	{
		int column_spcecial = 0;
		for (int count1 = 0; count1 <old.getrow()*old.getcolumn(); count1++)
		{

			if (*(old.Get1dPtr() + count1) == thr)
			{
				column_spcecial -= 1;
			}
			else
			{
				*(data[0] + column_spcecial) = *(old.Get1dPtr() + count1);

			}
			column_spcecial++;

		}
	}


}
template<class T>
MatrixIn<T>::MatrixIn(MatrixIn<T>& small, int row, int column, char size_Method)
{

	this->row = row;
	this->column = column;
	create();

	double row_dist = double(row);
	double column_dist = double(column);
	int row_src = small.getrow();
	int column_src = small.getcolumn();
	double y_amplitude = double(row_src) / row_dist;
	double x_amplitude = double(column_src) / column_dist;
	char type = size_Method;

	if (type == 'l')
	{
		for (int y_dist = 0; y_dist < row; y_dist++)
		{
			// deviation for origin point
			double y_src = (y_dist + 0.5)*y_amplitude - 0.5;
			int y1_src = floor(y_src);
			//set boundary condition 		
			// set initial value y0<0 =>y0=0
			if (y1_src < 0)
			{
				y_src = 0; y1_src = 0;
			}
			// set terminal value yn>length => yn=length
			if (y1_src >= row_src - 1)
			{
				y_src = row_src - 1; y1_src = row_src - 2;
			}

			int y2_src = y1_src + 1;

			for (int x_dist = 0; x_dist < column; x_dist++)
			{

				double x_src = (x_dist + 0.5)*x_amplitude - 0.5;
				int x1_src = floor(x_src);
				if (x1_src<0)
				{
					x_src = 0; x1_src = 0;
				}
				if (x1_src >= column_src - 1)
				{
					x_src = column_src - 1; x1_src = column_src - 2;
				}
				int x2_src = x1_src + 1;
				data[y_dist][x_dist] = (x2_src - x_src)*(y2_src - y_src)*(small.GetPtr()[y1_src][x1_src])
					+ (x_src - x1_src)*(y2_src - y_src)*(small.GetPtr()[y1_src][x2_src])
					+ (x2_src - x_src)*(y_src - y1_src)*(small.GetPtr()[y2_src][x1_src])
					+ (x_src - x1_src)*(y_src - y1_src)*(small.GetPtr()[y2_src][x2_src]);
			}
		}
	}
	else if (type == 'c')
	{
		for (int y_dist = 0; y_dist < row; y_dist++)
		{
			// deviation for origin point
			double y_src = (y_dist + 0.5)*y_amplitude - 0.5;

			int y1_src = floor(y_src);
			int y0_src, y2_src, y3_src;
			y_src = y_src - y1_src;
			//set boundary condition 		
			// set initial value P0=P1
			if (y1_src < 1)
			{
				y0_src = 0; y1_src = 0; y2_src = 1; y3_src = 2;

			}
			// set terminal value P3=P2
			else if (y1_src >= row_src - 2)
			{
				y0_src = row_src - 3; y1_src = row_src - 2; y2_src = y3_src = row_src - 1;

			}

			else
			{
				y0_src = y1_src - 1; y2_src = y1_src + 1; y3_src = y2_src + 1;
			}

			for (int x_dist = 0; x_dist < column; x_dist++)
			{

				double x_src = (x_dist + 0.5)*x_amplitude - 0.5;

				int x1_src = floor(x_src);
				int x0_src, x2_src, x3_src;
				x_src = x_src - x1_src;
				if (x1_src < 1)
				{
					x0_src = 0; x1_src = 0; x2_src = 1; x3_src = 2;

				}

				// set terminal value P3=P2
				else if (x1_src >= column_src - 2)
				{
					x0_src = column_src - 3; x1_src = column_src - 2; x2_src = x3_src = column_src - 1;


				}

				else
				{
					x0_src = x1_src - 1; x2_src = x1_src + 1; x3_src = x2_src + 1;
				}
				double y0, y1, y2, y3;
				y0 = small.GetPtr()[y0_src][x1_src] + 0.5*x_src*(small.GetPtr()[y0_src][x2_src] - small.GetPtr()[y0_src][x0_src] + x_src*(2 * small.GetPtr()[y0_src][x0_src] - 5 * small.GetPtr()[y0_src][x1_src] + 4 * small.GetPtr()[y0_src][x2_src]
					- small.GetPtr()[y0_src][x3_src] + x_src*(3 * (small.GetPtr()[y0_src][x1_src] - small.GetPtr()[y0_src][x2_src]) + small.GetPtr()[y0_src][x3_src] - small.GetPtr()[y0_src][x0_src])));
				y1 = small.GetPtr()[y1_src][x1_src] + 0.5*x_src*(small.GetPtr()[y1_src][x2_src] - small.GetPtr()[y1_src][x0_src] + x_src*(2 * small.GetPtr()[y1_src][x0_src] - 5 * small.GetPtr()[y1_src][x1_src] + 4 * small.GetPtr()[y1_src][x2_src]
					- small.GetPtr()[y1_src][x3_src] + x_src*(3 * (small.GetPtr()[y1_src][x1_src] - small.GetPtr()[y1_src][x2_src]) + small.GetPtr()[y1_src][x3_src] - small.GetPtr()[y1_src][x0_src])));
				y2 = small.GetPtr()[y2_src][x1_src] + 0.5*x_src*(small.GetPtr()[y2_src][x2_src] - small.GetPtr()[y2_src][x0_src] + x_src*(2 * small.GetPtr()[y2_src][x0_src] - 5 * small.GetPtr()[y2_src][x1_src] + 4 * small.GetPtr()[y2_src][x2_src]
					- small.GetPtr()[y2_src][x3_src] + x_src*(3 * (small.GetPtr()[y2_src][x1_src] - small.GetPtr()[y2_src][x2_src]) + small.GetPtr()[y2_src][x3_src] - small.GetPtr()[y2_src][x0_src])));
				y3 = small.GetPtr()[y3_src][x1_src] + 0.5*x_src*(small.GetPtr()[y3_src][x2_src] - small.GetPtr()[y3_src][x0_src] + x_src*(2 * small.GetPtr()[y3_src][x0_src] - 5 * small.GetPtr()[y3_src][x1_src] + 4 * small.GetPtr()[y3_src][x2_src]
					- small.GetPtr()[y3_src][x3_src] + x_src*(3 * (small.GetPtr()[y3_src][x1_src] - small.GetPtr()[y3_src][x2_src]) + small.GetPtr()[y3_src][x3_src] - small.GetPtr()[y3_src][x0_src])));
				data[y_dist][x_dist] = y1 + 0.5*y_src*(y2 - y0 + y_src*(2 * y0 - 5 * y1 + 4 * y2 - y3 + y_src*(3 * (y1 - y2) + y3 - y0)));

			}
		}
	}
	else
	{
		cerr << "no such method, pls enter bilinear/bicubic" << endl;
	}

}

template<class T>
T MatrixIn<T>::getMax()
{
	T max = (T)0;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j<column; j++)
		{
			if (max < data[i][j])
				max = data[i][j];
		}
	}
	return max;
}
template<class T>
T MatrixIn<T>::getsum()
{
	T		sum = (T)0;
	int		total_num = row*column;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j<column; j++)
		{
			sum += data[i][j];
		}
	}
	return sum;
}
template<class T>
T MatrixIn<T>::getMin()
{
	T min = (T)10;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j<column; j++)
		{
			if (min > data[i][j])
				min = data[i][j];
		}
	}
	return min;
}
template<class T>
T MatrixIn<T>::getavg()
{
	T		sum = getsum();
	T		result = sum / (row*column);
	return result;



}
template<class T>
T MatrixIn<T>::getstd()
{
	T		avg = getavg();
	T		sum = (T)0;
	T		result;
	int		total_num = row*column - 1;//N-1
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j<column; j++)
		{
			T x = data[i][j];
			sum = sum + (x - avg)*(x - avg);
		}
	}
	result = sqrt(sum / ((T)total_num));
	return result;
}
template<class T>
void MatrixIn<T>::iniData(T thr)
{
	for (int count1 = 0; count1 <row; count1++)
	{
		for (int count2 = 0; count2 < column; count2++)
		{
			if (data[count1][count2] < thr)
			{
				data[count1][count2] = (T)0;
			}

		}
	}
	return;
}
template<class T>
void MatrixIn<T>::outputMatrix()
{
	string name_01 = "target.txt";
	fstream fp01(name_01.c_str(), ios::out);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			if (j == column - 1)
				fp01 << data[i][j] << endl;
			else
				fp01 << data[i][j] << '\t';
		}
	}
	fp01.close();
}
template<class T>
int MatrixIn<T>::trim_count(char symbol, T thr)
{
	char jud = symbol;
	int number_cancel = 0;
	// threshold initialize 
	if (jud == '<')
	{
		number_cancel = 0;
		for (int count1 = 0; count1 < row; count1++)
		{
			for (int count2 = 0; count2 < column; count2++)
			{
				if (data[count1][count2] < (T)thr)
				{
					number_cancel++;
				}


			}
		}
	}
	else if (jud == '>')
	{
		number_cancel = 0;
		for (int count1 = 0; count1 < row; count1++)
		{
			for (int count2 = 0; count2 < column; count2++)
			{
				if (data[count1][count2] >(T)thr)
				{
					number_cancel++;
				}

			}
		}
	}
	else if (jud == '=')
	{
		number_cancel = 0;
		for (int count1 = 0; count1 < row; count1++)
		{
			for (int count2 = 0; count2 < column; count2++)
			{
				if (data[count1][count2] == (T)thr)
				{
					number_cancel++;
				}

			}
		}
	}
	else
	{
		cerr << "pls enter matrix name '>' '<' '=',threshold" << endl;
	}
	return number_cancel;
}
template<class T>
void MatrixIn<T>::Abs()
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j<column; j++)
		{
			if (data[i][j] < (T)0)
			{
				data[i][j] = data[i][j] * -1;
			}
		}
	}
	return;
}
template<class T>
void MatrixIn<T>::rotate(MatrixIn<T>& indata)
{
	del();
	row = indata.getcolumn(); //512
	column = indata.getrow(); //128		
	create();
	for (int i = 0; i < row; i++)
	{
		int y = indata.getrow() - 1;
		for (int j = 0; j < column; j++)
		{
			data[i][j] = indata[y][i];
			y--;
		}
	}
}

//-----------------------------------------
//------------ASQ definition---------------
//-----------------------------------------
long double ASQ::getHcW(MatrixIn<double>& indata, double dy)
{
	int data_height = indata.getrow();
	int data_width = indata.getcolumn();
	indata.Abs();
	double signal_max = indata.getMax();

	int count1, count2;
	for (count1 = 0; count1 < data_height; count1++)
	{

		for (count2 = 0; count2 < data_width; count2++)
		{

			indata.GetPtr()[count1][count2] = indata.GetPtr()[count1][count2] / signal_max;

		}

	}
	// set bar_number & size
	double set_barNum = (2 / dy) + 1;
	double hist_max = indata.getMax();
	double hist_min = indata.getMin();
	double range_bar = hist_max - hist_min;
	long double bar_width = round((range_bar / set_barNum)*pow(10, 7)) / pow(10, 7); // nearest to decimal point 7 pow(10, 7)

	MatrixIn<double> pdf(set_barNum, 2);
	for (count1 = 0; count1 < set_barNum; count1++)
	{
		int num_state = 0;

		if (count1 == set_barNum - 1)// calculation for the last bar of histgram
		{
			for (count2 = 0; count2 < data_height*data_width; count2++)
			{
				if (indata.GetPtr()[0][count2] >= (count1*bar_width) + hist_min)
				{
					num_state++;
				}
			}
		}
		else
		{
			for (count2 = 0; count2 < data_height*data_width; count2++)
			{
				if (indata.GetPtr()[0][count2] < (count1 + 1)*bar_width + hist_min  && indata.GetPtr()[0][count2] >= (count1*bar_width) + hist_min)
				{
					num_state++;
				}
			}

		}
		pdf.GetPtr()[count1][0] = num_state;
		pdf.GetPtr()[count1][1] = (hist_min + (hist_min + bar_width)) / 2 + count1*bar_width; //middle position of each bar

	}
	// get max & sum of state!!!
	double pdf_max = 0.0;
	double pdf_sum = 0.0;
	for (count1 = 0; count1<set_barNum; count1++)
	{
		pdf_sum = pdf_sum + pdf.GetPtr()[count1][0];
		if (pdf_max < pdf.GetPtr()[count1][0])
			pdf_max = pdf.GetPtr()[count1][0];
	}

	for (count1 = 0; count1 < set_barNum; count1++)
	{
		pdf.GetPtr()[count1][0] = (pdf.GetPtr()[count1][0] + pow(10, -7)*pdf_max) / pdf_sum;
	}

	//------------weight factor--------------
	MatrixIn<double> wf(1, set_barNum);
	for (count1 = 0; count1 < set_barNum; count1++)
	{
		*(wf.Get1dPtr() + count1) = pdf.GetPtr()[count1][1] * pdf.GetPtr()[count1][1];
	}
	//Weighted Shannon entropy
	// S=-sum(log2(H(i)))
	long double sum_result = 0;
	for (count1 = 0; count1 < set_barNum; count1++)
	{
		sum_result += (log2(pdf.GetPtr()[count1][0]))*pdf.GetPtr()[count1][0] * wf.GetPtr()[0][count1];
	}
	sum_result = sum_result * -1;
	//NaN judgment

	if (isnan(sum_result) == 1)
	{
		sum_result = 0;
	}
	return sum_result;
}

void ASQ::ASQcalculation(MatrixIn<double>& indata, MatrixIn<double>& mask)
{
	clock_t start, end, cm2_star, cm2end;
	const int ASQ_line_num = indata.getcolumn();
	const int ASQ_data_length = indata.getrow();
	//Breakpoint(ASQ_line_num); Breakpoint(ASQ_data_length);
	int row = indata.getrow();
	int column = indata.getcolumn();
	//Breakpoint(row); Breakpoint(column);
	//initial(all elem less than 0 -->0)
	indata.iniData(this->thr);
	const double scan_step = this->roi_width / indata.getcolumn();

	// Determining window size
	double y = 0.5*v*(pulselength / fs);
	int inte = round(window_size*pulselength);
	int inte1 = round(window_size*(y / scan_step));
	//Breakpoint(y); Breakpoint(inte); Breakpoint(inte1);
	int ddd = 0, ccc = 0;				//counter for CM2 matrix size

										// Matrix size calculation 

	for (int lateral_i = 0; lateral_i < (ASQ_line_num - inte1); lateral_i += round(inte1*WOR))
	{
		ccc++;
	}

	for (int axial_i = 0; axial_i < (ASQ_data_length - inte); axial_i += round(inte*WOR))
	{
		ddd++;
	}
	//Breakpoint(ddd); Breakpoint(ccc);
	//create CM2 matrix
	MatrixIn<double> Mat_CM2(ddd, ccc);
	//create rCM2 matrix
	MatrixIn<double> Mat_rCM2(ddd, ccc);
	//create red data
	MatrixIn<double> Mat_bluedata(ddd, ccc);
	//create blue data
	MatrixIn<double> Mat_reddata(ddd, ccc);

	//CM2 calculation
	cm2_star = clock();
	int CM_column = 0; int CM_row = 0;
	for (int lateral_i = 0; lateral_i < (ASQ_line_num - inte1); lateral_i += round(inte1*WOR))
	{
		CM_row = 0;
		for (int axial_i = 0; axial_i < (ASQ_data_length - inte); axial_i += round(inte*WOR))
		{

			MatrixIn<double> blocks(indata, axial_i, inte, lateral_i, inte1);

			//CM2 giving value
			Mat_CM2.GetPtr()[CM_row][CM_column] = getHcW(blocks, 0.04);

			// blocks filter (element>mean+4*standard will equal to 0) 
			long double blocks_thr = blocks.getavg() + 4 * blocks.getstd();

			MatrixIn<double> blocks_trim(blocks, '>', blocks_thr);

			//rCM2
			Mat_rCM2.GetPtr()[CM_row][CM_column] = getHcW(blocks_trim, 0.04);

			//threshold(weighted entropt : >0.95; CM2: <1.2)

			if ((Mat_CM2.GetPtr()[CM_row][CM_column] / Mat_rCM2.GetPtr()[CM_row][CM_column])>0.95)
			{

				Mat_reddata.GetPtr()[CM_row][CM_column] = Mat_CM2.GetPtr()[CM_row][CM_column];
				Mat_bluedata.GetPtr()[CM_row][CM_column] = double(0);
			}
			else
			{

				Mat_reddata.GetPtr()[CM_row][CM_column] = double(0);
				Mat_bluedata.GetPtr()[CM_row][CM_column] = Mat_rCM2.GetPtr()[CM_row][CM_column];
			}

			CM_row++;
			//release memory
		}
		CM_column++;
	}

	for (int i = 0; i<Mat_CM2.getrow(); i++)
		for (int j = 0; j < Mat_CM2.getcolumn(); j++)
		{

			Mat_CM2.GetPtr()[i][j] = Mat_reddata.GetPtr()[i][j] + Mat_bluedata.GetPtr()[i][j];
		}
	cm2end = clock();
	double cm2_dur = double(cm2end - cm2_star) / CLOCKS_PER_SEC;
	Breakpoint(cm2_dur);


	// interplotation
	MatrixIn<double> Mat_inte_CM2(Mat_CM2, row, column, 'c');
	MatrixIn<double> Mat_inte_bluedata(Mat_bluedata, row, column, 'l');
	MatrixIn<double> Mat_inte_reddata(Mat_reddata, row, column, 'l');

	// create roi matrix
	MatrixIn<double> Mat_ROI_CM2(row, column);
	for (int count1 = 0; count1<row; count1++)
		for (int count2 = 0; count2 < column; count2++)
		{
			if (mask.GetPtr()[count1][count2] == 0)
			{
				Mat_inte_bluedata.GetPtr()[count1][count2] = double(0);
				Mat_inte_reddata.GetPtr()[count1][count2] = double(0);
				Mat_ROI_CM2.GetPtr()[count1][count2] = double(0);
			}
			else
			{

				Mat_ROI_CM2.GetPtr()[count1][count2] = Mat_inte_CM2.GetPtr()[count1][count2];
			}
		}
	//eliminate elements which value as 0
	MatrixIn<double> Mat_inte1_bluedata(Mat_inte_bluedata, '=', 0);
	MatrixIn<double> Mat_inte1_reddata(Mat_inte_reddata, '=', 0);
	MatrixIn<double> Mat_ROI1_CM2(Mat_inte_CM2, '=', 0);
	//Breakpoint(Mat_ROI1_CM2.getcolumn());
	//output data
	
	this->CM2data_height = Mat_inte_CM2.getrow();
	this->CM2data_width = Mat_inte_CM2.getcolumn();
	this->CM2_data = new double*[CM2data_height];
	CM2_data[0] = new double[CM2data_height*CM2data_width];
	for (int i = 0; i < CM2data_height; i++)
	{
		CM2_data[i] = CM2_data[i - 1] + CM2data_width;
	}

	double bluelength = Mat_inte1_bluedata.getcolumn();
	double redlength = Mat_inte1_reddata.getcolumn();
	this->FD_ratio = bluelength / redlength;
	this->homo_index = 100 * (1 - FD_ratio);
	Mat_ROI1_CM2.Abs();
	this->CM2_ratio = Mat_ROI1_CM2.getavg();


}

void ASQ::loadfile()
{
	clock_t start, end;
	string name_01 = "rf_data.txt";
	string name_02 = "mask.txt";
	int data_height = 0, data_width = 0;
	stringstream ss_01;
	fstream fp01(name_01.c_str(), ios::in);
	if (fp01.fail())
	{
		cout << "file: " << name_01 << " can't open!!" << endl;
		cout << "pls check ur file name and press any key for leave~" << endl;
		system("pause");
		exit(1);
	}
	double num;
	string non;
	while (fp01.peek() != EOF)
	{
		getline(fp01, non);
		data_height++;

	}
	ss_01 << non;
	while (ss_01.peek() != EOF)
	{
		if (ss_01 >> num)
			data_width++;

	}
	ss_01.clear();
	fp01.close();

	if (data_height<1 || data_width<1)
	{
		cout << "pls check the data in file: " << name_01 << endl;
		cout << "It might be empty or unacceptable data type" << endl;
		system("pause");
		exit(1);
	}

	//Breakpoint(data_width); Breakpoint(data_height);


	double** mat_data = new double*[data_height];
	mat_data[0] = new double[data_height*data_width];
	for (int i = 1; i < data_height; i++)
		mat_data[i] = mat_data[i - 1] + data_width;

	fstream fp02(name_01.c_str(), ios::in);
	while (fp02.peek() != EOF)
	{
		for (int i = 0; i < data_height; i++)
			for (int j = 0; j < data_width; j++)
			{

				fp02 >> mat_data[i][j];
			}


	}
	fp02.close();
	
	indata.setnew(data_height, data_width, mat_data);

	double** mat_mask = new double*[data_height];
	mat_mask[0] = new double[data_height*data_width];
	for (int i = 1; i < data_height; i++)
		mat_mask[i] = mat_mask[i - 1] + data_width;
	fstream fp03(name_02.c_str(), ios::in);
	while (fp03.peek() != EOF)
	{
		for (int i = 0; i < data_height; i++)
			for (int j = 0; j < data_width; j++)
			{

				fp03 >> mat_mask[i][j];
			}


	}
	fp03.close();
	mask.setnew(data_height, data_width, mat_mask);
}

//-----------------------------------------
//------------Fan  -------------------
//-----------------------------------------

template<class T>
void Fanout<T>::setNewpara(int data_width, int data_height, double fanangle=60, double img_lenX=60, double img_lenY=120)
{
	this->data_width = data_width;
	this->data_height = data_height;
	this->fanangle = fanangle;
	this->img_length_X = img_lenX;
	this->img_length_Y = img_lenY;

}

template<class T>
void Fanout<T>::FanImgPara()
{
	double				img_pix_X = img_length_X / data_height; Breakpoint(img_pix_X); //128
	double				img_pix_Y = img_length_Y / data_width; Breakpoint(img_pix_Y);	//512
	double				radius_short = img_length_X / (2 * pi) / (fanangle / 360); Breakpoint(radius_short);
	double				radius_long = radius_short + img_length_Y; Breakpoint(radius_long);
	double				fanLengthX = 2 * radius_long* sin((fanangle*pi / 180.0) / 2); Breakpoint(fanLengthX);
	double				fanLengthY = radius_long - radius_short*cos((fanangle*pi / 180.0) / 2); Breakpoint(fanLengthY);

	double				halfPoint = data_height / 2; Breakpoint(halfPoint);
	double				angleInte = fanangle / halfPoint / 2; Breakpoint(angleInte);
	double				startAngle = -(halfPoint - 0.5)*angleInte; Breakpoint(startAngle);
						fanSizeX = ceil(fanLengthX / img_pix_X); Breakpoint(fanSizeX);
						fanSizeY = ceil(fanLengthY / img_pix_Y); Breakpoint(fanSizeY);

	double				oPixelX = ceil(double(fanSizeX) / 2); Breakpoint(oPixelX);
	double				oPointX = -(radius_long)*sin(fanangle*pi / 360); Breakpoint(oPointX);
	double				oPointY = -(radius_short)*cos(fanangle*pi / 360); Breakpoint(oPointY);


	temp_X = new double*[fanSizeY];
	temp_X[0] = new double[fanSizeY*fanSizeX];
	for (int i = 1; i < fanSizeY; i++)
	{
		temp_X[i] = temp_X[i - 1] + fanSizeX;
	}

	temp_Y = new double*[fanSizeY];
	temp_Y[0] = new double[fanSizeY*fanSizeX];

	for (int i = 1; i < fanSizeY; i++)
	{
		temp_Y[i] = temp_Y[i - 1] + fanSizeX;
	}


	fanImg = new T*[fanSizeY];
	fanImg[0] = new T[fanSizeX*fanSizeY];
	for (int i = 1; i < fanSizeY; i++)
	{
		fanImg[i] = fanImg[i - 1] + fanSizeX;
	}


	for (int j = 0; j < fanSizeY; j++)
	{
		for (int i = 0; i < fanSizeX; i++)
		{
			double pointX = (double(i + 1) - oPixelX)*img_pix_X;
			double pointY = oPointY - (j)*img_pix_Y;
			double r = sqrt(pointX*pointX + pointY*pointY);
			double angle = asin(pointX / r)*180.0 / pi;

			temp_X[j][i] = (angle - startAngle) / angleInte;
			temp_Y[j][i] = (r - radius_short) / (img_pix_Y)+1;


		}

	}
	
	


}
template<class T>
void Fanout<T>::calFanImg(T* pBufU)
{
	
	//2D matrix reshape to 128 *512
	//MatrixIn<T>		temp(pBufU, data_height, data_width); //128*512
	int count1 = 0;
	MatrixIn<int>   temp_Xfr(data_height, data_width);
	for(int j = 0; j < data_width; j++)
		for (int i = 0; i<data_height; i++)
		{
			temp_Xfr.GetPtr()[i][j] = (int)pBufU[count1];
			count1++;
		}
	
	MatrixIn<int>	indata(data_width, data_height);	//512*128

	for (int i = 0; i < data_width; i++) //512
	{
		int x = 127; //128
		for (int j = 0; j < data_height; j++) //128
		{
			indata.GetPtr()[i][j] = temp_Xfr.GetPtr()[x][i];
			x = x - 1;
		}
	}
	
	//MatrixIn<int> finalresult(fanSizeY, fanSizeX);

	for (int j = 0; j < fanSizeY; j++)
	{
		for (int i = 0; i < fanSizeX; i++)
		{
			if ((temp_X[j][i] >= 0 && temp_X[j][i] <= (data_height - 1)) && (temp_Y[j][i] >= 0 && temp_Y[j][i] <= (data_width - 1)))
			{
				int x1 = floor(temp_X[j][i]);
				int x2 = ceil(temp_X[j][i]);
				double x = temp_X[j][i] - x1;

				int y1 = floor(temp_Y[j][i]);
				int y2 = ceil(temp_Y[j][i]);
				double y = temp_Y[j][i] - y1;
				int result =round(indata.GetPtr()[y1][x1] * (1.0 - x)*(1.0 - y) + indata.GetPtr()[y1][x2] * x*(1.0 - y) + indata.GetPtr()[y2][x1] * (1.0 - x)* y + indata.GetPtr()[y2][x2] * x*y);
				//finalresult.GetPtr()[j][i] = result;
				fanImg[j][i] = (T)result;

			}
			else
			{
				//finalresult.GetPtr()[j][i] = 0;
				fanImg[j][i] = (T)0;
			}
		}

	}
	//finalresult.outputMatrix();
	
}

template<class T>
void Fanout<T>::canFanImg_color(T* bmpdata)
{
	//2D matrix reshape to 128 *512
	//MatrixIn<T>		temp(pBufU, data_height, data_width); //128*512
	int count1 = 0;
	MatrixIn<int>   temp_Xfr(data_height, data_width,3);
	for (int j = 0; j < data_width; j++)
		for (int i = 0; i<data_height; i++)
		{
			temp_Xfr.GetPtr()[i][j] = (int)pBufU[count1];
			count1++;
		}

	MatrixIn<int>	indata(data_width, data_height*3);	//512*128

	for (int i = 0; i < data_width; i++) //512
	{
		int x = 383; //128
		for (int j = 0; j < data_height*3; j++) //128
		{
			indata.GetPtr()[i][j] = temp_Xfr.GetPtr()[x][i];
			x = x - 1;
		}
	}

	//MatrixIn<int> finalresult(fanSizeY, fanSizeX);

	for (int j = 0; j < fanSizeY; j++)
	{
		for (int i = 0; i < fanSizeX; i++)
		{
			if ((temp_X[j][i] >= 0 && temp_X[j][i] <= (data_height - 1)) && (temp_Y[j][i] >= 0 && temp_Y[j][i] <= (data_width - 1)))
			{
				int x1 = floor(temp_X[j][i]);
				int x2 = ceil(temp_X[j][i]);
				double x = temp_X[j][i] - x1;

				int y1 = floor(temp_Y[j][i]);
				int y2 = ceil(temp_Y[j][i]);
				double y = temp_Y[j][i] - y1;
				int result = round(indata.GetPtr()[y1][x1] * (1.0 - x)*(1.0 - y) + indata.GetPtr()[y1][x2] * x*(1.0 - y) + indata.GetPtr()[y2][x1] * (1.0 - x)* y + indata.GetPtr()[y2][x2] * x*y);
				//finalresult.GetPtr()[j][i] = result;
				fanImg[j][i] = (T)result;

			}
			else
			{
				//finalresult.GetPtr()[j][i] = 0;
				fanImg[j][i] = (T)0;
			}
		}

	}
	//finalresult.outputMatrix();
}

//-----------------------------------------
//------------ASQ instant------------------
//-----------------------------------------
static ASQ asq;
//for use
DLLexport void getASQ_all(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width, double**& rf_data, double**& mask_data, int rf_height, int rf_width)
{
	asq.getASQ_all(fc, fs, velocity, pulselength, threshold, windowsize, overlap_rate, roi_width, rf_data, mask_data, rf_height, rf_width);
}
//for test
DLLexport void getASQ(int fc, int fs, int velocity, int pulselength, int threshold, double windowsize, double overlap_rate, int roi_width)
{
	asq.getASQ(fc, fs, velocity, pulselength, threshold, windowsize, overlap_rate, roi_width);
}
DLLexport double getHomo_Index()
{
	return asq.homo_index;
}
DLLexport double getCM2_ratio()
{
	return asq.CM2_ratio;
}
DLLexport double getFD_ratio()
{
	return asq.FD_ratio;
}
DLLexport double** getCM2()
{
	return asq.CM2_data;
}

static Fanout<BYTE> fan_Uint;

DLLexport void setFanPara()
{
	fan_Uint.FanImgPara();
}
DLLexport BYTE** getFanPtr(BYTE* pBufB)
{
	fan_Uint.calFanImg(pBufB);
	return fan_Uint.fanImg;
}
//DLLexport BYTE getElm(int row,int column)
//{
//	
//	return fan_Uint.fanImg[row][column];
//}
