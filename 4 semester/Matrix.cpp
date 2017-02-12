#include <iostream>
#include <stdlib.h>

using namespace std;

class Matrix {

	private:
	int m;
	int n;

	int *data;

	public:
	Matrix(int rows, int cols, int *init) {
	
		if ((rows < 1) || (cols < 1)) {
			exit(-1);
		}	

		m = rows;
		n = cols;

		int length = m * n;

		data = new int[length];

		for(int i = 0; i < length; i++) {
			data[i] = init[i];
		}

	}

	int getRows() {
		return m;
	}

	int getColumns() {
		return n;
	}

	int getElement(int row, int col) {

		if ((row < 0) || (row >= m) || (col < 0) || (col >= n)) {
			exit(-1);
		}

		return data[row * n + col];

	}

	Matrix multiply(Matrix& M) {

		if (n != M.m) {
			exit(-1);
		}

		Matrix result(m, M.n, new int[m * M.n]);

		int current = 0;

		for(int i = 0; i < m; i++) {
			for(int j = 0; j < M.n; j++) {
			
				result.data[current] = 0;

				for(int t = 0; t < n; t++) {
					result.data[current] += getElement(i, t) * M.getElement(t, j);
				}
			
				current++;

			}
		}

		return result;

	}

};

ostream& operator <<(ostream& out, Matrix M) {

	int rows = M.getRows();
	int cols = M.getColumns();

	for(int i = 0; i < rows; i++) {
		
		out << M.getElement(i, 0);

		for(int j = 1; j < cols; j++) {
			out << ' ' << M.getElement(i, j);
		}

		out << '\n';

	}

	return out;

}

int main() {

	int ar[] = {2, 5, 88, -9};

	Matrix A(2, 2, ar);

	int arr[] = {0, 1, 0, 1, 5, 5};

	Matrix B(2, 3, arr);
		
	cout << A << " * \n" << B << " = \n" << A.multiply(B);

	return 0;

}
