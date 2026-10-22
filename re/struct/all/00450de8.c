
undefined4 FUN_00450de8(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x44);
  if ((((*(int *)(iVar1 + 0x84) + 0xc < param_2) &&
       (param_2 < *(int *)(iVar1 + 0x84) + *(int *)(iVar1 + 0x7c) + -0xc)) &&
      (*(int *)(iVar1 + 0x88) + 0xc < param_3)) &&
     (param_3 < *(int *)(iVar1 + 0x88) + *(int *)(iVar1 + 0x80) + -0xc)) {
    return CONCAT31((int3)((uint)iVar1 >> 8),1);
  }
  return 0;
}

