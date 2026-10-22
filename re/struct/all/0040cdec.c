
void FUN_0040cdec(int param_1)

{
  int iVar1;
  
  (**(code **)(**(int **)(param_1 + 0x80) + 0x6c))(*(int **)(param_1 + 0x80),0);
  Sound_PlaySample("Sfx\\Int\\BookClse.wav");
  iVar1 = FUN_0041bbb8(*(undefined4 *)PTR_DAT_0045a174);
  *(int *)(iVar1 + 0x38) = *(int *)(iVar1 + 0x38) + 1;
  FUN_0041ca5c(*(undefined4 *)PTR_DAT_0045a174);
  return;
}

