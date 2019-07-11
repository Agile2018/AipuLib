#ifndef Facemat_h
#define Facemat_h

#include <vector>

using namespace std;

class Facemat
{
public:
	Facemat();
	~Facemat();
	void SetChannels(int channelsInput) {
		channels = channelsInput;
	}
	int GetChannels() {
		return channels;
	}

	void SetWidth(int widthInput) {
		width = widthInput;
	}
	int GetWidth() {
		return width;
	}

	void SetHeight(int heightInput) {
		height = heightInput;
	}
	int GetHeight() {
		return height;
	}

	void SetData(vector<unsigned char> dataInput) {
		data = dataInput;
	}
	vector<unsigned char> GetData() {
		return data;
	}

	void SetCols(int colsInput) {
		cols = colsInput;
	}

	int GetCols() {
		return cols;
	}

	void SetRows(int rowsInput) {
		rows = rowsInput;
	}

	int GetRows() {
		return rows;
	}

	void SetElemSize1(int element) {
		elemSize1 = element;
	}
	int GetElemSize1() {
		return elemSize1;
	}

	void SetStep(int stepInput) {
		step = stepInput;
	}
	int GetStep() {
		return step;
	}

	void SetStep1(int step1Input) {
		step1 = step1Input;
	}

	int GetStep1() {
		return step1;
	}

private:
	int channels; 
	int width;
	int height;
	int step1;
	vector<unsigned char> data;
	int cols; 
	int rows; 
	int elemSize1; 
	int step;
};


#endif // !Facemat_h

