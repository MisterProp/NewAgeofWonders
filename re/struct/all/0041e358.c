
void FUN_0041e358(int param_1)

{
  int iVar1;
  
  (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),0);
  Sound_PlaySample("Sfx\\Int\\BookClse.wav");
  if (*(char *)(*(int *)(param_1 + 0x4c) + 0x75) == '\0') {
    AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0);
  }
  if (*(char *)(*(int *)(param_1 + 0x4c) + 0x75) != '\0') {
    *(undefined4 *)(param_1 + 0x150) = 0x14;
    iVar1 = FUN_0041bbb8(param_1);
    *(int *)(iVar1 + 0x38) = *(int *)(iVar1 + 0x38) + 1;
    FUN_0041ca5c(param_1);
  }
  return;
}

