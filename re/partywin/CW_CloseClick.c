
void FUN_00406018(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0x44);
  iVar2 = *(int *)(iVar1 + 0xd4);
  *(int *)(iVar2 + 0x1c) =
       (*(int *)(*(int *)(iVar1 + 0x1d8) + 0x7c) - *(int *)(iVar1 + 0x84)) - *(int *)(iVar1 + 0x7c);
  *(undefined4 *)(iVar2 + 0x20) = *(undefined4 *)(iVar1 + 0x88);
  return;
}

