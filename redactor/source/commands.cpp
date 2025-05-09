#include "commands.h"


class SplittedString {
private:
  int size = 0;
	char **parts = nullptr;

	static int CountChar(char* str, char sep) {
		int cnt = 0;
		int length = strlen(str);
		for (int i = 0; i < length; ++i) {
			if (str[i] == sep) {
				++cnt;
			}
		}

		return cnt;
	}

public:
	SplittedString(char *str, char del = ' ') {
    std::string temp = "";
	  int partscount = CountChar(str, del) + 1;

	  parts = new char*[partscount];
	  int count = 0;

		for(int i = 0; i < strlen(str); i++){
      if (str[i] != del) {
        temp += str[i];
      } else {
	    	parts[count] = new char[temp.size() + 1];
	    	strcpy(parts[count], temp.data());
	    	count++;
        temp = "";
      }
    }

	  parts[count] = new char[temp.size() + 1];
	  strcpy(parts[count], temp.data());

    size = partscount;
	}

	int getSize() const {
		return size;
	}

	const char * operator[](int index) const {
		if (index < 0 || index >= size) {
			throw std::runtime_error("index out of range");
		}
		return parts[index];
	}

	~SplittedString() {
		for (int i = 0; i < size; i++) {
			delete[] parts[i];
		}
		delete[] parts;
	}
};

static void MoveWithWord(Window &w, int n, int pos, int coef) {
	int newpos = pos;
	for (int i = 0; i < n;) {

		if (w.GetChar() == ' ') {
			w.AddPointer(coef);
			if (w.GetPointerPosition() == 0) {
				w.MovePointerInPos(0);
				return;
			}
			if (w.GetPosibility() == false) {
				w.AddPointer((-1) * coef);
				w.MovePointerInPos(newpos);
				return;
			}
		}else {
			char* word;
			int count = 1;
			w.ReturnWord(&word);
			w.AddPointer(coef);
			do {
				if (w.GetPointerPosition() == 0) {
					w.MovePointerInPos(0);
					return;
				}
				if (w.GetPosibility() == false) {
					w.AddPointer((-1) * coef);
					w.MovePointerInPos(newpos);
					return;
				}
				w.AddPointer(coef);
				count++;
			}while (w.GetChar() != ' ') ;
			i++;
			newpos = w.GetPointerPosition();
			delete [] word;
		}
	}
	return;
}

static char* AnotherCase(const char *str, int Pos) {
  char* ptr = new char[strlen(str) + 1];
	int j = 0;
  for (int i = 0; str[i] != '\0'; ++i) {
		if (Pos == 1) {
			if ((str[i] >= 'a') && (str[i] <= 'z')) {
				ptr[j++] = str[i] - 32;
			}else {
				ptr[j++] = str[i];
			}
		}else {
			if ((str[i] >= 'A') && (str[i] <= 'Z')) {
				ptr[j++] = str[i] + 32;
			}else {
				ptr[j++] = str[i];
			}
		}
  }
  ptr[j] = '\0';
	return ptr;
}

static bool isValidNumber(const char* str) {
	if (str == nullptr || *str == '\0') return false;

	if (*str == '-' || *str == '+') ++str;

	while (*str) {
			if (!std::isdigit(*str)) return false;
			++str;
	}

	return true;
}

static int IsNumber(const char* num) {
	if (isValidNumber(num)){
		return atoi(num);
	}else {
		throw std::runtime_error("invalid number");
	}
}
void ProcessingCommands(char* input, Window &w) {
	SplittedString spl(input);

	if (spl.getSize() > 3) {
		throw std::runtime_error("incorrect format");
	}

	if (strcmp(spl[0], "q") == 0) {
		// system("clear");
		return;
	}

	if (strcmp(spl[0], "add") == 0 && spl.getSize() == 2) {
		w.MovePointerInPos(w.LastPos());
		w.WriteInPos(spl[1]);
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "p") == 0 && spl.getSize() == 1) {
		if (w.GetPointerPosition() <= w.LastPos()) {
			w.MoveData(1);
		}
		w.WriteInPos(" ");
		w.AddPointer(1);
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "move") == 0 && spl.getSize() == 2) {
		w.AddPointer(IsNumber(spl[1]));
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "moveto") == 0 && spl.getSize() == 2) {
		w.MovePointerInPos(IsNumber(spl[1]));
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "insert") == 0 && spl.getSize() == 3) {
		int pos = w.GetPointerPosition();
		int newpos = IsNumber(spl[1]);
		w.MovePointerInPos(newpos);
		if (newpos <= w.LastPos()) {
			int length = strlen(spl[2]);
			w.MoveData(length);
		}
		w.WriteInPos(spl[2]);
		w.MovePointerInPos(pos);
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "put") == 0 && spl.getSize() == 2) {
		int length = strlen(spl[1]);

		w.MoveData(length);
		w.WriteInPos(spl[1]);
		w.AddPointer(length);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "remove") == 0 && spl.getSize() == 3) {
		int start = IsNumber(spl[1]);
		int end = IsNumber(spl[2]);
		int length = end - start + 1;
		if (w.InBounds(start, end) == false || length < 0) {
			std::cout << '\a';
			return;
		}

		w.MovePointerInPos(start);
		char* str = new char[length + 1];
		memset(str, ' ', sizeof(char) * length);
		str[length] = '\0';
		w.WriteInPos(str);

		delete [] str;
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "del") == 0 && spl.getSize() == 1) {
		if (w.GetPointerPosition() == 0) {
			w.PrintWindow();
			return;
		}
		w.MoveDataLeft(1);
		w.AddPointer(-1);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "del") == 0 && spl.getSize() == 2) {
		int n = IsNumber(spl[1]);

		if (w.GetPointerPosition() < n) {
			w.PrintWindow();
			return;
		}
		w.MoveDataLeft(n);
		w.AddPointer((-1) * n);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "moverw") == 0 && spl.getSize() == 2) {
		int n = IsNumber(spl[1]);

		MoveWithWord(w, n, w.GetPointerPosition(), 1);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "movelw") == 0 && spl.getSize() == 2) {
		int n = IsNumber(spl[1]);

		MoveWithWord(w, n, w.GetPointerPosition(), -1);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "upcase") == 0 && spl.getSize() == 1) {
		int pos = w.GetPointerPosition();
		int length = 0;
		int PosOfFirstChar = w.CountWord(&length);
		char* word;
		w.ReturnWord(&word);

		char* Upperword = AnotherCase(word, 1);
		w.MovePointerInPos(PosOfFirstChar);
		w.WriteInPos(Upperword);

		delete [] Upperword;
		delete [] word;
		w.MovePointerInPos(pos);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "lowcase") == 0 && spl.getSize() == 1) {
		int pos = w.GetPointerPosition();
		int length = 0;
		int PosOfFirstChar = w.CountWord(&length);

		char* word;
		w.ReturnWord(&word);

		char* LowerWord = AnotherCase(word, -1);
		w.MovePointerInPos(PosOfFirstChar);
		w.WriteInPos(LowerWord);

		delete [] LowerWord;
		delete [] word;
		w.MovePointerInPos(pos);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "next") == 0 && spl.getSize() == 1) {
		w.MoveToEnd();
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "find") == 0 && spl.getSize() == 2) {
		w.FindWord(spl[1]);
		if (w.GetHighlitedSize() == 0) {
			std::cout << '\a';
			return;
		}
		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "replace") == 0 && spl.getSize() == 3) {
		w.FindWord(spl[1]);
		if (w.GetHighlitedSize() == 0) {
			std::cout << '\a';
			return;
		}
		w.Replace(spl[1], spl[2]);
		w.FindWord(spl[2]);

		w.PrintWindow();
		return;
	}

	if (strcmp(spl[0], "save") == 0 && spl.getSize() == 2) {
		w.Save(spl[1]);
		std::cout << "file saved\n";
		return;
	}

	if (strcmp(spl[0], "load") == 0 && spl.getSize() == 2) {
		w.Load(spl[1]);
		w.PrintWindow();
		return;
	}

	std::cout << "unknown command" << std::endl;

	return;
}

void InputCommands(Window &window) {
	std::string input = "";

	while (input != "q") {
		std::getline(std::cin, input);

		char *inputc = new char[input.size() + 1];
		strcpy(inputc, input.c_str());

		ProcessingCommands(inputc, window);

		delete [] inputc;
	}
	return;

}
