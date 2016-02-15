#include "../OFLib/ROM.h"

void Test(ROM &rom)
{
	rom.DumpMapGraphics("../Quests/FF1/Graphics/Maps");

	//rom.DumpMapData("../Quests/FF1/Maps");
}

void main()
{
	ROM rom("finalfantasy1.nes");
	rom.ExportFull();

	//Test(rom);
}

