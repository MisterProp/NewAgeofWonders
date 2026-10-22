
void FUN_0044a658(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(*(int *)(param_1 + 0x110) + 0x120);
  if (iVar1 != -1) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x114) + 0x114);
    uVar3 = (**(code **)(*piVar2 + 0x18))(piVar2,iVar1);
    *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0) = uVar3;
    FUN_00407db8(*(undefined4 *)PTR_DAT_0045a53c);
  }
  return;
}

