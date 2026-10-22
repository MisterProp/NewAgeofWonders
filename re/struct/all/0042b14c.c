
undefined4 FUN_0042b14c(int param_1,char param_2)

{
  undefined *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 local_14;
  
  puVar1 = PTR_DAT_0045a454;
  iVar3 = -1;
  local_14 = 0;
  iVar4 = 0;
  while (iVar5 = *(int *)(*(int *)(*(int *)puVar1 + 0x44) + 0x1bc), iVar4 < *(int *)(iVar5 + 8)) {
    iVar2 = Classes_TList_Get_23EDC2EF(iVar5,iVar4);
    iVar5 = iVar4;
    if (param_2 == *(char *)(iVar2 + 0x1c)) {
      iVar5 = *(int *)(*(int *)(*(int *)(*(int *)puVar1 + 0x44) + 0x1bc) + 8);
      iVar3 = iVar4;
    }
    iVar4 = iVar5 + 1;
  }
  if (-1 < iVar3) {
    System__LStrAsg_51F89FF7
              (*(int *)(*(int *)puVar1 + 0x44) + 400,
               *(undefined4 *)(*(int *)(param_1 + 0x94) + 0x90));
    local_14 = aowDPlayComp_TGDPlay_Connect_23EDC2EF(*(undefined4 *)(*(int *)puVar1 + 0x44),iVar3);
  }
  return local_14;
}

