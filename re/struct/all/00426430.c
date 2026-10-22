
void FUN_00426430(int param_1,int param_2)

{
  undefined4 uVar1;
  
  if (*(char *)(param_2 + 0xb0) == '\0') {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,2);
  }
  else {
    FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
  }
  *(undefined1 *)(*(int *)(param_1 + 0x54) + 0x2d) = 0;
  if (*(int *)(param_1 + 0x27c) == 0) {
    FUN_004248e4(param_1);
    uVar1 = Engine_TIntegerList_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x28c),*(int *)(param_2 + 0xc) + -0x14);
    FUN_00425a28(param_1,uVar1);
    uVar1 = Engine_TIntegerList_Get_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x28c),*(int *)(param_2 + 0xc) + -0x14);
    FUN_00425780(param_1,uVar1);
    return;
  }
  uVar1 = Engine_TIntegerList_Get_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x28c),*(int *)(param_2 + 0xc) + -0x14);
  FUN_00425a28(param_1,uVar1);
  return;
}

