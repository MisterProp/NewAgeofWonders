// /AoWEPACK.dpl : 5575c80c

undefined4 AoWE_TProductionQueue_GetItem_23EDC2EF(int *param_1,int param_2)

{
  int iVar1;
  
                    /* 0x5c80c  2387  AoWE.TProductionQueue.GetItem@23EDC2EF */
  if (-1 < param_2) {
    iVar1 = (**(code **)(*param_1 + 0x54))();
    if (param_2 < iVar1) {
      return *(undefined4 *)(*(int *)(param_1[2] + 4) + param_2 * 4);
    }
  }
  return 0;
}

