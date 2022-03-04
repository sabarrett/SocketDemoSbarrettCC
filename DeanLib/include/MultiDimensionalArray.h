#pragma once

#include "Trackable.h"

template < class T >
class MultiDimensionalArray :public Trackable
{
public:
	explicit MultiDimensionalArray(unsigned int numRows, unsigned int numCols, const T& initialValue)
		:mNumRows(numRows)
		,mNumCols(numCols)
	{
		mArray = new T[mNumRows*mNumCols];
		for (unsigned int i = 0; i < mNumRows*mNumCols; i++)
		{
			mArray[i] = initialValue;
		}
	
	}

	~MultiDimensionalArray()
	{
		delete [] mArray;
	}

	T* get(unsigned int index)
	{
		if (index < mNumRows * mNumCols)
		{
			return &(mArray[index]);
		}
		else
			return NULL;
	}

	T* get(unsigned int colNum, unsigned int rowNum)
	{
		return get(convertCoordsToIndex(colNum,rowNum));
	}

	void set(unsigned int index, const T& val) { *(get(index)) = val; }
	void set(unsigned int colNum, unsigned int rowNum, const T& val) { *(get(colNum,rowNum)) = val; }

	void convertIndexToCoords(unsigned int index, unsigned int& xOut, unsigned int& yOut)
	{
		xOut = index % mNumCols;
		yOut = index / mNumCols;
	}

	unsigned int convertCoordsToIndex(const unsigned int& xIn, const unsigned int& yIn)
	{
		unsigned int index = yIn * mNumCols + xIn;
		return index;
	}

private:
	T* mArray;
	unsigned int mNumRows;
	unsigned int mNumCols;


};
