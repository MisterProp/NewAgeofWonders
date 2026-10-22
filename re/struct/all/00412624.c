
void FUN_00412624(int param_1,int param_2)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  
  if (param_2 < 0) {
LAB_004126df:
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),0xffffffff);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x2e0),0xffffffff);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x2d8),0xffffffff);
    (**(code **)(**(int **)(param_1 + 0x290) + 0x6c))(*(int **)(param_1 + 0x290),0);
    (**(code **)(**(int **)(param_1 + 0x304) + 0x6c))(*(int **)(param_1 + 0x304),0);
    return;
  }
  iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x5c) + 0x114) + 0x14))();
  if (iVar3 <= param_2) goto LAB_004126df;
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x2e0),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x2d8),param_2);
  cVar2 = AoWSetup_TSetupControl_GetHost_23EDC2EF(*(undefined4 *)(param_1 + 0x33c));
  if (cVar2 != '\0') {
    piVar1 = *(int **)(*(int *)(param_1 + 0x5c) + 0x114);
    iVar4 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x5c) + 0x120));
    iVar3 = *(int *)(*(int *)(param_1 + 0x33c) + 0x34);
    if (iVar4 != *(int *)(iVar3 + 0xc)) {
      uVar5 = CONCAT31((int3)((uint)iVar3 >> 8),1);
      goto LAB_004126a3;
    }
  }
  uVar5 = 0;
LAB_004126a3:
  (**(code **)(**(int **)(param_1 + 0x290) + 0x6c))(*(int **)(param_1 + 0x290),uVar5);
  piVar1 = *(int **)(*(int *)(param_1 + 0x5c) + 0x114);
  uVar5 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x5c) + 0x120));
  uVar5 = AoWSetup_TSetupControl_CanSetHostRemoteID_23EDC2EF(*(undefined4 *)(param_1 + 0x33c),uVar5)
  ;
  (**(code **)(**(int **)(param_1 + 0x304) + 0x6c))(*(int **)(param_1 + 0x304),uVar5);
  return;
}

