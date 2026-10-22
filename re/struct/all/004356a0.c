
void FUN_004356a0(int param_1,undefined4 param_2,undefined4 *param_3,undefined4 *param_4)

{
  int iVar1;
  
  switch(param_2) {
  case 0:
    iVar1 = *(int *)(param_1 + 0x74);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 1:
  case 7:
    iVar1 = *(int *)(param_1 + 0x80);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 2:
    iVar1 = *(int *)(param_1 + 0x8c);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 3:
    iVar1 = *(int *)(param_1 + 0x98);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 4:
    iVar1 = *(int *)(param_1 + 0xa4);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 5:
    iVar1 = *(int *)(param_1 + 0xb0);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
    break;
  case 6:
    iVar1 = *(int *)(param_1 + 0xbc);
    *param_3 = *(undefined4 *)(iVar1 + 0x84);
    *param_4 = *(undefined4 *)(iVar1 + 0x88);
  }
  return;
}

