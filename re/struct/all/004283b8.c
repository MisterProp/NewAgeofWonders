
void FUN_004283b8(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,int *param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int local_2c;
  int local_20;
  
  if (*(int *)(param_3 + 8) != 0) {
    iVar1 = param_5[2];
    iVar2 = *param_5;
    Engine_TIntegerList_Get_23EDC2EF(param_3,0);
    local_2c = *(int *)(param_3 + 8) + -1;
    if (0 < local_2c) {
      local_20 = 1;
      do {
        Engine_TIntegerList_Get_23EDC2EF(param_3,local_20);
        iVar3 = System__ROUND_51F89FF7();
        iVar4 = System__ROUND_51F89FF7();
        iVar5 = System__ROUND_51F89FF7();
        iVar6 = System__ROUND_51F89FF7();
        ImageLib_DrawImageLine_F4100AF0
                  (param_2,iVar3 + *param_5,param_5[3] - iVar4,param_4,1,param_5[3] - iVar6,
                   *param_5 + iVar5);
        local_20 = local_20 + 1;
        local_2c = local_2c + -1;
      } while (local_2c != 0);
    }
    if (*(int *)(param_3 + 8) == 1) {
      iVar3 = System__ROUND_51F89FF7();
      System__ROUND_51F89FF7();
      ImageLib_DrawImageLine_F4100AF0
                (param_2,*param_5,param_5[3] - iVar3,param_4,1,param_5[3] - iVar3,
                 *param_5 + (iVar1 - iVar2) + -1);
    }
  }
  return;
}

