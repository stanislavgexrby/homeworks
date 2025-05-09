#include "pointer.h"

Pointer::Pointer()
  : pos(0), inbounds(true) {};

Pointer::~Pointer(){};

void Pointer::Add(int x, int Width, int Heigh) {
  if (pos + x < 0 || pos + x >= Width * Heigh) {
    inbounds = false;
    return;
  }
  pos += x;
  inbounds = true;
};

void Pointer::Move(int x, int Width) {
  pos = 0;
  Add(x, Width);
};

int Pointer::GetPos() const {
  return pos;
};

bool Pointer::GetInbounds() const {
  return inbounds;
};