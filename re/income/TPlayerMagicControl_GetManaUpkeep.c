

int AoWE_TPlayerMagicControl_GetManaUpkeep_23EDC2EF(int param_1)



{

  int iVar1;

  int iVar2;

  int iVar3;

  

                    /* 0x7c964  1204  AoWE.TPlayerMagicControl.GetManaUpkeep@23EDC2EF */

  iVar3 = 0;

  iVar1 = (**(code **)(**(int **)(param_1 + 0x3c) + 0x54))();

  iVar1 = iVar1 + -1;

  if (-1 < iVar1) {

    do {

      iVar2 = AoWE_TEnchantmentList_GetEnchantments_23EDC2EF(*(undefined4 *)(param_1 + 0x3c),iVar1);

      iVar3 = iVar3 + *(int *)(iVar2 + 0x10);

      iVar1 = iVar1 + -1;

    } while (iVar1 != -1);

  }

  iVar1 = AoWE_TGlobalMagicControl_GetEnchantmentUpkeep_23EDC2EF

                    (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x188),

                     *(undefined1 *)(*(int *)(param_1 + 0x10) + 0xa6));

  return iVar3 + iVar1;

}




