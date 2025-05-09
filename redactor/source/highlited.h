#pragma once

class Highlighted {
private:
  int* pos;
  int length;
  int size = 0;
public:
  Highlighted();
  ~Highlighted();

  void AddHighlited(int position = 0);
  void AddHighlitedLength(int _length = 0);

  bool IncludeHighlited(int position = 0);
  void ClearHighlited();

  int GetHighlitedSize();
};