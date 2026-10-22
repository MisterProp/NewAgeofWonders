
undefined4 FUN_00455eac(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  uVar2 = 0;
  iVar3 = *(int *)(*(int *)(*(int *)(param_1 + 0x44) + 0xd0) + 8) + -1;
  while (-1 < iVar3) {
    iVar1 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 0x44) + 0xd0),iVar3);
    if (((iVar1 == *(int *)(*(int *)PTR_DAT_0045a5bc + 0x44)) || (*(char *)(iVar1 + 0x104) == '\0'))
       || (*(char *)(iVar1 + 0x75) == '\0')) {
      iVar3 = iVar3 + -1;
    }
    else {
      uVar2 = 1;
      iVar3 = -1;
    }
  }
  return uVar2;
}

