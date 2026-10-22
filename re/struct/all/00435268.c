
void FUN_00435268(int param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined2 uStack_e;
  
  AoWE_TCombatViewer_Finalize_23EDC2EF(*(undefined4 *)(param_1 + 0x144));
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0);
  pcVar1 = *(code **)(param_1 + 0x148);
  uVar2 = *(undefined4 *)(param_1 + 0x14c);
  piVar3 = *(int **)(param_1 + 0x150);
  *(undefined4 *)(param_1 + 0x148) = 0;
  *(undefined4 *)(param_1 + 0x14c) = 0;
  *(undefined4 *)(param_1 + 0x150) = 0;
  uStack_e = (short)((uint)pcVar1 >> 0x10);
  if (uStack_e != 0) {
    (*pcVar1)(uVar2,piVar3);
  }
  if (piVar3 != (int *)0x0) {
    (**(code **)(*piVar3 + 0x2c))();
  }
  return;
}

