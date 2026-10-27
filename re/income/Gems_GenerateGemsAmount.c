

void Gems_TGems_GenerateGemsAmount_23EDC2EF(int param_1)



{

  char cVar1;

  int iVar2;

  

                    /* 0xc5010  7742  Gems.TGems.GenerateGemsAmount@23EDC2EF */

  if (*(char *)(param_1 + 0x20) == '\x04') {

    cVar1 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,3);

    *(char *)(param_1 + 0x20) = cVar1 + '\x01';

  }

  cVar1 = *(char *)(param_1 + 0x20);

  if (cVar1 == '\x01') {

    iVar2 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,0x19);

    *(int *)(param_1 + 0x1c) = iVar2 + 0x19;

    return;

  }

  if (cVar1 == '\x02') {

    iVar2 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,0x19);

    *(int *)(param_1 + 0x1c) = iVar2 + 0x32;

    return;

  }

  if (cVar1 == '\x03') {

    iVar2 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,0x32);

    *(int *)(param_1 + 0x1c) = iVar2 + 100;

  }

  return;

}




