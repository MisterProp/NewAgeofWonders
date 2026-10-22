
void FUN_00456aac(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x44) + 0xd4);
  if ((*(char *)(iVar1 + 0x28) == '\x03') && (*(char *)(*(int *)(param_2 + 0xd4) + 0x28) == '\x03'))
  {
    iVar2 = *(int *)(*(int *)PTR_DAT_0045a5bc + 0x44);
    *(int *)(iVar1 + 0x24) =
         *(int *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x80) + *(int *)(param_2 + 0x80);
    *(undefined1 *)(iVar2 + 0x2d) = 0;
    if (*(char *)(iVar2 + 0x75) == '\0') {
      *(undefined4 *)(*(int *)(param_2 + 0xd4) + 0x1c) = 0;
    }
    else {
      *(undefined4 *)(*(int *)(param_2 + 0xd4) + 0x1c) =
           *(undefined4 *)(*(int *)(iVar2 + 0xd4) + 0x1c);
    }
    *(undefined1 *)(param_2 + 0x2d) = 0;
  }
  return;
}

