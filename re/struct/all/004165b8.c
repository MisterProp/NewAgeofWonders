
void FUN_004165b8(int param_1)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar4 = *(int *)(*(int *)(param_1 + 0x44) + 0x120);
  if (iVar4 != -1) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x44) + 0x114);
    uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,iVar4);
    piVar1 = *(int **)(*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c);
    cVar2 = (**(code **)(*piVar1 + 0x4c))(piVar1,uVar3);
    if (cVar2 != '\0') {
      piVar1 = *(int **)(*(int *)(*(int *)(param_1 + 0x2e4) + 0x40) + 0x2c);
      iVar4 = (**(code **)(*piVar1 + 0x84))(piVar1,uVar3);
      iVar5 = (**(code **)(**(int **)(param_1 + 0x2e4) + 0x144))(*(int **)(param_1 + 0x2e4),uVar3);
      if (iVar4 == iVar5) {
        return;
      }
    }
    (**(code **)(**(int **)(param_1 + 0x2e4) + 0x98))(*(int **)(param_1 + 0x2e4),uVar3);
    FUN_0041610c(param_1);
    FUN_00415f94(param_1);
  }
  return;
}

