
void FUN_0043c290(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                    (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x1c),param_2);
  if (iVar1 != 0) {
    *(undefined4 *)(param_1 + 0x2c4) = param_2;
    if (*(char *)(iVar1 + 0xc) == '\0') {
      FUN_0043c43c(param_1,7);
    }
    else {
      FUN_0043c43c(param_1,8);
      AoWE_TTutorialControl_ShowMessage_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x3a,1,1);
    }
  }
  return;
}

