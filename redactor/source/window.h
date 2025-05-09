#pragma once

#include "pointer.h"
#include "dataset.h"
#include "highlited.h"

#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>

class Window {
private:
	Highlighted _highlighted;
	DataSet _data;
	Pointer _pointer;

	char* window = nullptr;
	int width = 80;
	int heigh = 25;
public:
	Window(int x = 25, int y = 80);
	~Window();

	void PrintWindow();

	void WriteInPos(const char* str);
	void MoveData(int length = 0);
	void MoveDataLeft(int length = 0);
	void Replace(const char* str1, const char* str2);

	void ReturnWord(char** word);
	int CountWord(int* count);
	char GetChar();
	void FindWord(const char* word);

	void AddPointer(int x = 0);
	void MovePointerInPos(int pos = 0);
	void MoveToEnd();
	bool GetPosibility();
	int GetPointerPosition();
	bool InBounds(int start = 0, int end = 0);

	void Save(const char* name);
	void Load(const char* name);
	void Clear();

	int LastPos();
	int GetHighlitedSize();
};

