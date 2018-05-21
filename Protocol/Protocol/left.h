#include <iostream>
#include <string>
#include <Windows.h>
#include <conio.h>
#include <math.h>

using namespace std;


class memory {
public:
	int a0 = 0;
	int a1 = 0;
	int a2 = 0;
	int a3 = 0;

	int unwritten[4] = { 0,0,0,0 };
	int **written = new int *[4]; //yeah, it's complicated, get over it

	memory() {
		written[0] = &a0;
		written[1] = &a1;
		written[2] = &a2;
		written[3] = &a3;
	}

	//easy way to update memmory
	void updateMemory(int address) {
		written[0][address] = unwritten[address];
	}

	//used for debugging
	void printUnwritten() {
		cout << endl << endl << " unwritten: ";
		for (int i : unwritten)
			cout << i << ", ";
	}

	void entryText() {
		cout << "Commands to use this protocol:\n\n";
		cout << "1. Hit esc to close this program\n";
		cout << "2. Hit r, after that CPU number, and address line to read off\n";
		cout << "3. Hit w, and after that CPU number, adn address line to write on\n\n";
	}

	void print() {
		entryText();
		printUnwritten();

		cout << endl;

		cout << "                               " << "________________________\n";
		cout << "                               " << "| address: a0 | data: " << a0 << " |" << endl;
		cout << "                               " << "| address: a1 | data: " << a1 << " |" << endl;
		cout << "                               " << "| address: a2 | data: " << a2 << " |" << endl;
		cout << "                               " << "| address: a3 | data: " << a3 << " |" << endl;
		cout << "                               " << "________________________\n\n\n";
	}

};

struct cache {
	//first row
	char state1 = 'I';
	string address1 = "    ";
	int value1 = 0;

	//second row
	char state2 = 'I';
	string address2 = "    ";
	int value2 = 0;
};

class CPU{
public:
	cache cache;
};

class Lines {
public:

	enum color { normal = 7, red = 12, blue = 9, purple = 13 };

	void dataBus(bool s = false) {
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (s)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::red);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 75; j++) {
				if (j < 8)
					cout << " ";
				else
					cout << "*";
			}
			if (i == 0)
				cout << "Data Bus";
			cout << endl;
		}
		cout << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::normal);
	}
	void addressBus(bool s = false) {
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (s)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::blue);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 75; j++) {
				if (j < 8)
					cout << " ";
				else
					cout << "*";
			}
			if (i == 0)
				cout << "Address Bus";
			cout << endl;
		}
		cout << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::normal);
	}
	void Shared(bool s = false) {
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (s)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::purple);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 75; j++) {
				if (j < 8)
					cout << " ";
				else
					cout << "*";
			}
			if (i == 0)
				cout << "Shared";
			cout << endl;
		}
		cout << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color::normal);
	}
};

class PC {
public:

	CPU CPU[3];
	memory mem;
	Lines lines;

	bool adLine=false, shLine=false, dtLine=false;

	int lastOperationCPU;
	int lastOperationAddress;
	int RW;

	PC() {}

	void print() {
		mem.print();

		lines.dataBus(dtLine);
		lines.addressBus(adLine);
		lines.Shared(shLine);

		cout << endl << endl;
		cout << "      _________________         _________________         _________________\n";

		cout << "      | " << CPU[0].cache.state1 << " | " << CPU[0].cache.address1 << " | " << CPU[0].cache.value1 << " |         ";
		cout << " | " << CPU[1].cache.state1 << " | " << CPU[1].cache.address1 << " | " << CPU[1].cache.value1 << " |         ";
		cout << " | " << CPU[2].cache.state1 << " | " << CPU[2].cache.address1 << " | " << CPU[2].cache.value1 << " |\n";

		cout << "      | " << CPU[0].cache.state2 << " | " << CPU[0].cache.address2 << " | " << CPU[0].cache.value2 << " |         ";
		cout << " | " << CPU[1].cache.state2 << " | " << CPU[1].cache.address2 << " | " << CPU[1].cache.value2 << " |         ";
		cout << " | " << CPU[2].cache.state2 << " | " << CPU[2].cache.address2 << " | " << CPU[2].cache.value2 << " |\n";

		cout << "      _________________         _________________         _________________\n";

		if (RW == 1)
			cout << "Write CPU: " << lastOperationCPU << " with address: " << lastOperationAddress << endl;
		else if (RW == 2)
			cout << "Read CPU: " << lastOperationCPU << " with address: " << lastOperationAddress << endl;


	}

	void read(char address, char CPU) {


		int i = CPU - 48;
		int add = address - 48;

		this->lastOperationAddress = add;
		this->lastOperationCPU = i;

		int left[2];
		switch (i) {
		case 0: left[0] = 1; left[1] = 2; break;
		case 1: left[0] = 0; left[1] = 2; break;
		case 2: left[0] = 0; left[1] = 1; break;
		}


		//cpu operation::

		if (address == 48 | address == 50) {
			switch (this->CPU[i].cache.state1) {
			case 'I':
				this->CPU[i].cache.value1 = mem.unwritten[add];

				mem.updateMemory(add);

				if (isEx(left))
					this->CPU[i].cache.state1 = 'F';
				else
					this->CPU[i].cache.state1 = 'E'; //change if needed

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;
			default:

				this->CPU[i].cache.value1 = mem.unwritten[add];
				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;
			}
		}
		else if (address == 49 | address == 51) {
			switch (this->CPU[i].cache.state2) {
			case 'I':
				this->CPU[i].cache.value2 = mem.unwritten[add];

				mem.updateMemory(add);

				if (isEx(left))
					this->CPU[i].cache.state2 = 'F';
				else
					this->CPU[i].cache.state2 = 'E'; //change if needed

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;
			default:

				this->CPU[i].cache.value2 = mem.unwritten[add];
				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;
			}
		}

		//bus operations:

		for (int t : left) {
			if (address == 48 | address == 50)
				if (this->CPU[t].cache.address1[2] == address) {
					if (this->CPU[t].cache.state1 != 'I')
						this->CPU[t].cache.state1 = 'S';
				}
			if (address == 49 | address == 51)
				if (this->CPU[t].cache.address2[2] == address) {
					if (this->CPU[t].cache.state2 != 'I')
						this->CPU[t].cache.state2 = 'S';
				}

		}
		RW = 2;
		system("cls");
		print();
	}

	bool isEx(int *arr) {

		if (this->CPU[arr[0]].cache.state1 == 'E') return 1;
		if (this->CPU[arr[1]].cache.state1 == 'E') return 1;

		return 0;
	}

	int findmax() {
		int max = this->CPU[0].cache.value1;
		int find[5] = {
			this->CPU[1].cache.value1,
			this->CPU[2].cache.value1,
			this->CPU[0].cache.value2,
			this->CPU[1].cache.value2,
			this->CPU[2].cache.value2
		};
		for (int i = 0; i < 5; i++)
			max = max(max, find[i]);
		return max;
	}

	void write(char address, char CPU) {

		int i = CPU - 48;
		int add = address - 48;

		this->lastOperationAddress = add;
		this->lastOperationCPU = i;

		/* ************************************************************ */
		//TODO!!!														//
		//guess i have to do bus read... fuck it						//
		/* ************************************************************ */

		read(address, CPU);
		RW = 0;
		system("cls");
		print();
		cout << "Read before write\n" << endl;

		system("pause");


		mem.unwritten[add] = findmax() + 1;


		//cpu operation::

		if (address == 48 | address == 50) {
			switch (this->CPU[i].cache.state1) {
			case 'I':
				this->CPU[i].cache.state1 = 'M';
				this->CPU[i].cache.value1 = findmax() + 1;

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;

			case 'F':
				this->CPU[i].cache.state1 = 'M';
				this->CPU[i].cache.value1 = findmax() + 1;

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;

			case 'S':
				this->CPU[i].cache.state1 = 'M';
				this->CPU[i].cache.value1 = findmax() + 1;

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;

			case 'M':
				//no need to change to M
				this->CPU[i].cache.value1 = findmax() + 1;

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";

				break;

			case 'E':
				this->CPU[i].cache.state1 = 'M';
				this->CPU[i].cache.value1 = findmax() + 1;

				if (add == 2)
					this->CPU[i].cache.address1 = " a2 ";
				else
					this->CPU[i].cache.address1 = " a0 ";


				break;

			}
		}
		else {
			switch (this->CPU[i].cache.state2) {
			case 'I':
				this->CPU[i].cache.state2 = 'M';
				this->CPU[i].cache.value2 = findmax() + 1;

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;

			case 'F':
				this->CPU[i].cache.state2 = 'M';
				this->CPU[i].cache.value2 = findmax() + 1;

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;

			case 'S':
				this->CPU[i].cache.state2 = 'M';
				this->CPU[i].cache.value2 = findmax() + 1;

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;

			case 'M':
				this->CPU[i].cache.state2 = 'M';
				this->CPU[i].cache.value2 = findmax() + 1;

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;

			case 'E':
				this->CPU[i].cache.state2 = 'M';
				this->CPU[i].cache.value2 = findmax() + 1;

				if (add == 3)
					this->CPU[i].cache.address2 = " a3 ";
				else
					this->CPU[i].cache.address2 = " a1 ";

				break;
			}
		}

		//so far so good

		//bus operations
		int left[2];
		switch (i) {

		case 0: left[0] = 1; left[1] = 2; break;
		case 1: left[0] = 0; left[1] = 2; break;
		case 2: left[0] = 0; left[1] = 1; break;

		}

		//leftover cpu's

		for (int t : left)
			if (address == 48 | address == 50)
				if (this->CPU[t].cache.address1[2] == address)
					this->CPU[t].cache.state1 = 'I'; //for all 5 states


		for (int t : left)
			if (address == 49 | address == 51)
				if (this->CPU[t].cache.address2[2] == address)
					this->CPU[t].cache.state2 = 'I'; //same deal going on around here

		RW = 1;
		system("cls");
		print();
	}
};
