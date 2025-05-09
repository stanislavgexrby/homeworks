#include "dataset.h"

int DataSet::GetSize() {
  return size;
}

DataSet::DataSet()
  :size(0), data(nullptr) {};

DataSet::~DataSet(){
  delete [] data;
}

void DataSet::AddData(const char* str, int pos) {
  int length = strlen(str);
  if (pos + length > size) {
    char* newdata = new char[pos + length + 1];
    memset(newdata, ' ', sizeof(char) * (pos + length));
    if (size != 0) {
      for (int k = 0; k < size; ++k) {
        newdata[k] = data[k];
      }
    }
    for (int i = 0; i < length; ++i) {
      newdata[pos + i] = str[i];
    }
    delete [] data;
    this->data = newdata;
    this->size = pos + length;
  }else {
    for (int j = 0; j < length; ++j) {
      data[pos + j] = str[j];
    }
  }
}

const char DataSet::operator[](int index) const {
  if (index < 0 || index >= size) {
  	throw std::runtime_error("index out of range");
  }
  return data[index];
}

char* DataSet::GetData() {
  return data;
}

void DataSet::PrintData() {
  for (int i = 0; i < size; ++i) {
    std::cout << data[i];
  }
  std::cout << '\n' << size << '\n';
}

void DataSet::Replace(char* str) {
  if (size != 0) {
    delete [] data;
  }
  this->data = str;
  this->size = strlen(str);
}

void DataSet::Clear() {
  if (size != 0) {
    delete [] data;
  }
  this->data = nullptr;
  this->size = 0;
}
