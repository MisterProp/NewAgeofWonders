

undefined4 AoWE_THero_GetUpkeep_23EDC2EF(undefined4 param_1)



{

  int iVar1;

  

                    /* 0x872c0  663  AoWE.THero.GetUpkeep@23EDC2EF */

  iVar1 = AoWE_THero_GetLevel_23EDC2EF(param_1);

  return CONCAT31((int3)((uint)(iVar1 * 2) >> 8),(char)(iVar1 * 2) + '\x05');

}




