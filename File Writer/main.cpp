#include <fstream>
#include <iostream>

void writeBasicAddonFile(const char* fileName);

typedef unsigned char byte;

int main(const int argc, const char* const argv[])
{
	writeBasicAddonFile("basic.addon");
	return 0;
}

void writeBasicAddonFile(const char* fileName)
{
	byte* header = new byte[6];
	header[0] = 0x00;
	header[1] = 0x01;
	header[2] = 0x00;
	header[3] = 0x00;
	header[4] = 0x00;
	header[5] = 0x01;
	byte id[1][6];
	id[0][0] = 0x02;//Type
	id[0][1] = 0x00;//Flags
	id[0][2] = 0x00;//ID
	id[0][3] = 0x00;
	id[0][4] = 0x00;
	id[0][5] = 0x01;
	const char* names[1];
	names[0] = "Static Cube";
	int nameLenghts[1];
	nameLenghts[0] = 12;

	std::ofstream baseFile(fileName, std::ios::binary);
	baseFile.write((const char*)header, 6);
	baseFile.write((const char*)id[0], 6);
	baseFile.write((const char*)names[0], nameLenghts[0]);
	baseFile.close();
}

void writeCubeFile(const char* fileName)
{

}