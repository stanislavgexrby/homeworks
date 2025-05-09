#include "highlited.h"

Highlighted::Highlighted()
  :size(0), pos(nullptr), length(0) {}

Highlighted::~Highlighted() {
  if (size != 0) {
    delete [] pos;
  }
};

void Highlighted::AddHighlited(int position) {
  int* New = new int[size + 1];
  for (int i = 0; i < size; ++i) {
    New[i] = pos[i];
  }
  New[size] = position;
  delete [] pos;
  this->pos = New;
  this->size = size + 1;
};

void Highlighted::AddHighlitedLength(int _length) {
  this->length = _length;
}

bool Highlighted::IncludeHighlited(int position) {
  for (int i = 0; i < size; ++i) {
    if (position >= pos[i] && position < pos[i] + length) {
      return true;
    }
  }
  return false;
};

void Highlighted::ClearHighlited() {
  if (size != 0) {
    delete [] pos;
  }
  this->pos = nullptr;
  this->length = 0;
  this->size = 0;
};

int Highlighted::GetHighlitedSize() {
  return size;
};
