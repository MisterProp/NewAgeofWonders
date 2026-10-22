
void FUN_00424984(int param_1)

{
  int *piVar1;
  int iVar2;
  
  *(undefined4 *)(param_1 + 0x278) = 0xffffffff;
  *(undefined1 *)(*(int *)(param_1 + 0x54) + 0x2d) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x44) + 0xa4) = 0;
  *(undefined4 *)(*(int *)(*(int *)(param_1 + 0x174) + 0xd4) + 0x20) = 10;
  iVar2 = 0x18;
  do {
    piVar1 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar2);
    (**(code **)(*piVar1 + 0xac))(piVar1,0);
    *(undefined4 *)(piVar1[0x35] + 0x20) = 10;
    iVar2 = iVar2 + 1;
  } while (iVar2 != 0x24);
  iVar2 = 0x24;
  do {
    piVar1 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x290),iVar2);
    (**(code **)(*piVar1 + 0xac))(piVar1,0);
    *(undefined4 *)(piVar1[0x35] + 0x20) = 0xb9;
    iVar2 = iVar2 + 1;
  } while (iVar2 != 0x30);
  return;
}

