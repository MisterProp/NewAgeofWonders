
void FUN_0040e26c(int param_1,int *param_2)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = *(int *)(*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) + 0x20);
  if ((*(char *)(param_1 + 0x1c0) == '\x03') &&
     (param_2[3] + *(int *)(param_1 + 0x1b0) == *(int *)(param_1 + 0x1c4))) {
    uVar4 = 1;
  }
  else {
    uVar4 = 0;
  }
  (**(code **)(*param_2 + 0xac))(param_2,uVar4);
  if ((char)param_2[0x2c] == '\0') {
    iVar2 = (**(code **)(**(int **)(iVar3 + 0x14) + 0x54))();
    if (param_2[3] + *(int *)(param_1 + 0x1b0) < iVar2) {
      iVar3 = Engine_TIntegerList_IndexOf_23EDC2EF
                        (*(undefined4 *)(iVar3 + 0x30),param_2[3] + *(int *)(param_1 + 0x1b0));
      if (iVar3 == -1) {
        uVar1 = 1;
        goto LAB_0040e2e5;
      }
    }
  }
  uVar1 = 0;
LAB_0040e2e5:
  *(undefined1 *)(param_2 + 0x30) = uVar1;
  return;
}

