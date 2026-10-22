
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0042b3c8(int param_1)

{
  int iVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = FUN_0042b14c(param_1,3);
  if (iVar1 == 0) {
    aowDPlayComp_TGDPlay_SetPlayerName_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44),
               *(undefined4 *)(*(int *)(param_1 + 0x90) + 0x90));
    _DAT_0045b170 = 0;
    (**(code **)(**(int **)(param_1 + 0x98) + 0x6c))(*(int **)(param_1 + 0x98),0);
    (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
    (**(code **)(**(int **)(*(int *)(param_1 + 0x48) + 0x114) + 0x40))();
    *(undefined1 *)(*(int *)(param_1 + 0x48) + 0x2e) = 0;
  }
  return;
}

