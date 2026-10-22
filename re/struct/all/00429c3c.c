
void FUN_00429c3c(int param_1)

{
  int iVar1;
  
  *(undefined4 *)(param_1 + 0x54) = 300;
  iVar1 = *(int *)(param_1 + 0x58);
  if (iVar1 == 0) {
    AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),1);
    *(int *)(param_1 + 0x58) = *(int *)(param_1 + 0x58) + 1;
    return;
  }
  if (iVar1 == 1) {
    FUN_00429b10(param_1);
    FUN_0041a150(*(undefined4 *)PTR_DAT_0045a434,0);
    return;
  }
  if (iVar1 == 10) {
    Forms_TCustomForm_Close_23EDC2EF(*(undefined4 *)PTR_DAT_0045a168);
  }
  return;
}

