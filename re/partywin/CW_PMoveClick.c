
undefined * FUN_00406848(int param_1)

{
  int iVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar2 = PTR_DAT_0045a420;
  if (*(char *)(*(int *)(param_1 + 0x15c) + 0xb0) == '\0') {
    iVar1 = *(int *)(*(undefined **)PTR_DAT_0045a420 + 0xa4);
    puVar3 = *(undefined **)PTR_DAT_0045a420;
    if ((*(byte *)(iVar1 + 0x28) & 1) != 0) {
      *(byte *)(iVar1 + 0x28) = *(byte *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) + 0x28) & 0xfe;
      puVar3 = puVar2;
    }
  }
  else {
    iVar1 = *(int *)(*(undefined **)PTR_DAT_0045a420 + 0xa4);
    puVar3 = *(undefined **)PTR_DAT_0045a420;
    if ((*(byte *)(iVar1 + 0x28) & 1) == 0) {
      puVar2 = (undefined *)
               (CONCAT31((int3)((uint)*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) >> 8),
                         *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) + 0x28)) | 1);
      *(char *)(iVar1 + 0x28) = (char)puVar2;
      return puVar2;
    }
  }
  return puVar3;
}

