#include "window.h"

Window::Window(int x, int y)
    : width(y), heigh(x), _pointer(), _data(), _highlighted() {
    window = new char [x * y];
    memset(window, ' ', sizeof(char) * x * y);
    PrintWindow();
};

Window::~Window() {
    delete [] window;
};

void Window::WriteInPos(const char* str) {
    int size = strlen(str);
    int pos = _pointer.GetPos();

    _data.AddData(str, pos);
    for (int i = 0; i < size; ++i) {
        window[pos + i] =  str[i];
        _pointer.Add(1, width, heigh);
        if (_pointer.GetInbounds() == false) {
            break;
        }
    }
    _pointer.Move(pos, width);
};

void Window::MoveData(int length) {
    int Pos = _pointer.GetPos();
    int LengthOfData = _data.GetSize();
    if (LengthOfData == 0 || LengthOfData <= Pos) {
        return;
    }
    char* Data = _data.GetData();
    char* NewData = new char[LengthOfData + length + 1];
    memset(NewData, ' ', sizeof(char) * (LengthOfData + length));
    NewData[LengthOfData + length] = '\0';
    for (int i = 0; i < Pos; ++i) {
        NewData[i] = Data[i];
    }
    for (int j = LengthOfData; j >= Pos; --j) {
        NewData[j + length - 1] = Data[j - 1];
        window[j + length] = window[j];
        window[j] = ' ';
    }
    _data.Replace(NewData);
};

void Window::MoveDataLeft(int length) {
    int pos = _pointer.GetPos();
    int LengthOfData = _data.GetSize();
    if (LengthOfData < length || pos < length) {
        return;
    }
    char* Data = _data.GetData();
    int cur = pos - length;
    if (pos < LengthOfData) {
        char* NewData = new char[LengthOfData - length + 1] {};
        memset(NewData, ' ', sizeof(char) * (LengthOfData - length));
        for (int i = 0; i < cur; ++i) {
            NewData[i] = Data[i];
        }
        for (int j = cur; j < LengthOfData - length; ++j) {
            NewData[j] = Data[j + length];
            window[j] = window[j + length];
            window[j + length] = ' ';
        }
        _data.Replace(NewData);
        return;
    }else {
        if (cur < LengthOfData) {
            char* NewData = new char[cur + 1] {};
            memset(NewData, ' ', sizeof(char) * (cur));
            for (int i = 0; i < cur; ++i) {
                NewData[i] = Data[i];
            }
            for (int j = cur; j < pos; ++j) {
                window[j] = ' ';
            }
            _data.Replace(NewData);
            return;
        }else {
            return;
        }

    }
};


int Window::CountWord(int* count) {
    int pos = _pointer.GetPos();
    int buf = pos;
    while (window[pos] != ' ') {
        if (pos == width * heigh) {
            (*count)++;
            break;
        }
        pos++;
        (*count)++;
    }
    if (buf == 0 && pos != buf) {
        return buf;
    }
    buf--;
    while (window[buf] != ' ') {
        if (buf == 0) {
            (*count)++;
            return buf;
        }
        buf--;
        (*count)++;
    }
    return buf + 1;
};

void Window::ReturnWord(char** word) {
    int length = 0;
    int PosOfFirstChar = CountWord(&length);
    (*word) = new char[length + 1];
    for (int i = 0; i < length; ++i) {
        (*word)[i] = window[PosOfFirstChar + i];
    }
    (*word)[length] = '\0';
};

char Window::GetChar() {
    return window[_pointer.GetPos()];
}

void Window::FindWord(const char* word) {
    _highlighted.ClearHighlited();
    int length = strlen(word);
    _highlighted.AddHighlitedLength(length);
    int i;
    for (i = 0; i < _data.GetSize() - length + 1; ++i) {
        if (_data[i] == word[0]) {
            if (length == 1) {
                _highlighted.AddHighlited(i);
                continue;
            }

            bool check = true;
            for (int j = 1; j < length; ++j) {
                if (_data[i + j] != word[j]) {
                    check = false;
                }
            }
            if (check) {
                _highlighted.AddHighlited(i);
                i += length - 1;
            }
        }
    }
}

void Window::AddPointer(int x) {
    _pointer.Add(x, width, heigh);
}

void Window::MovePointerInPos(int pos) {
    _pointer.Move(pos, width);
}

void Window::MoveToEnd() {
    int LengthToEnd = width - (_pointer.GetPos() % width);
    _pointer.Add(LengthToEnd, width);
    _data.AddData("\n", _pointer.GetPos() - 1);
}

bool Window::GetPosibility() {
    return _pointer.GetInbounds();
}

int Window::GetPointerPosition() {
    return _pointer.GetPos();
}

int Window::LastPos() {
    return _data.GetSize();
}

int Window::GetHighlitedSize() {
    return _highlighted.GetHighlitedSize();
}

bool Window::InBounds(int start, int end) {
    if (start < 0 || end > width * heigh) {
        return false;
    }
    return true;
}

void Window::Replace(const char* str1, const char* str2) {
    int pos = _pointer.GetPos();
    int length1 = strlen(str1);
    int length2 = strlen(str2);
    int i;
    for (i = 0; i < _data.GetSize() - length1 + 1; ++i) {
        if (_data[i] == str1[0]) {
            if (length1 == 1) {
                MovePointerInPos(i + length1);
                MoveDataLeft(length1);
                MovePointerInPos(i);
                MoveData(length2);
		        WriteInPos(str2);
                MovePointerInPos(i);
                i += length2 - 1;
                continue;
            }

            bool check = true;
            for (int j = 1; j < length1; ++j) {
                if (_data[i + j] != str1[j]) {
                    check = false;
                }
            }
            if (check) {
                MovePointerInPos(i + length1);
                MoveDataLeft(length1);
                MovePointerInPos(i);
                MoveData(length2);
		        WriteInPos(str2);
                MovePointerInPos(i);
                i += length2 - 1;
            }
        }
    }
    MovePointerInPos(pos);
}

void Window::Save(const char* name) {
    std::ofstream _file(name);
    if (_file.is_open()) {
        // _file << _data.GetData();
        _file.write(_data.GetData(), _data.GetSize());
        _file.close();
    } else {
        std::cerr << "Error opening file\n";
    }
}

void Window::Load(const char* name) {
    Clear();
    std::ifstream _file(name);
    char buf[2] {};
    if ( _file.is_open() ) {
        char c;
        while (_file) {
            c = _file.get();
            if (c == '\n') {
                MoveToEnd();
            }else {
                if (c != EOF) {
                    buf[0] = c;
                    WriteInPos(buf);
                    AddPointer(1);
                }
            }
        }
    }
    MovePointerInPos(0);
    _file.close();
}

void Window::Clear() {
    delete [] window;
    char* _window = new char [width * heigh];
    memset(_window, ' ', sizeof(char) * width * heigh);
    this->window = _window;
    MovePointerInPos(0);
}