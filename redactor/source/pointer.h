#pragma once

struct Pointer {
	public:
		Pointer();
		~Pointer();

		void Add(int x = 0, int Width = 80, int Heigh = 25);
		void Move(int x = 0, int Width = 80);

		int GetPos() const;
		bool GetInbounds() const;

	private:
		int pos = 0;
		bool inbounds = true;
};