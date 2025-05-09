#pragma once

#include <cstring>
#include <iostream>
#include <stdexcept>

class DataSet {
private:
    int size = 0;
    char* data = nullptr;
public:
	DataSet();
	~DataSet();
  int GetSize();
  void AddData(const char* str = nullptr, int pos = 0);
	const char operator[](int index) const;
	char* GetData();
	void PrintData();
	void Replace(char* str);
	void Clear();
};