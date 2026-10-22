
void FUN_004537a8(int *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = Controls_TControl_GetClientWidth_23EDC2EF(param_1);
  iVar1 = param_1[0xe];
  iVar5 = Controls_TControl_GetClientHeight_23EDC2EF(param_1);
  iVar5 = param_1[0xf] - iVar5;
  piVar2 = *(int **)(param_2 + 8);
  if (piVar2[2] - *piVar2 < iVar5 + 0x280) {
    iVar3 = *(int *)(param_2 + 4);
    if (((iVar3 == 1) || (iVar3 == 4)) || (iVar3 == 7)) {
      *piVar2 = (piVar2[2] + -0x280) - (iVar1 - iVar4);
    }
    else {
      piVar2[2] = (iVar1 - iVar4) + *piVar2 + 0x280;
    }
    *(undefined4 *)(param_2 + 0xc) = 1;
  }
  if (piVar2[3] - piVar2[1] < iVar5 + 0x1e0) {
    if (*(int *)(param_2 + 4) - 3U < 3) {
      piVar2[1] = (piVar2[3] + -0x1e0) - iVar5;
    }
    else {
      piVar2[3] = iVar5 + piVar2[1] + 0x1e0;
    }
    *(undefined4 *)(param_2 + 0xc) = 1;
  }
  (**(code **)(*param_1 + -0x10))(param_1,param_2);
  return;
}

