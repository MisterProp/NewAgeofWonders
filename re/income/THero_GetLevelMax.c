

int AoWE_THero_GetLevelMax_23EDC2EF(void)



{

  int iVar1;

  int iVar2;

  

                    /* 0x87628  655  AoWE.THero.GetLevelMax@23EDC2EF */

  iVar2 = 0x1e;

  if ((((AoWE_AoWHSMap_6B957A58 != 0) && (*(int *)(AoWE_AoWHSMap_6B957A58 + 0x18c) != 0)) &&

      (0 < *(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x18c) + 0x18))) &&

     (iVar1 = *(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x18c) + 0x18), iVar1 < 0x1e)) {

    iVar2 = iVar1;

  }

  return iVar2;

}




