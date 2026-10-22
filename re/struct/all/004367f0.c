
void FUN_004367f0(int param_1)

{
  code *pcVar1;
  undefined4 uVar2;
  undefined1 uVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uStack_16;
  
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0);
  iVar7 = *(int *)(*(int *)(param_1 + 0x88) + 8);
  if (-1 < iVar7 + -1) {
    iVar6 = 0;
    do {
      uVar3 = Engine_TByteList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x88),iVar6);
      piVar4 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF
                                (*(undefined4 *)(*(int *)(param_1 + 0x80) + 0x18),uVar3);
      iVar5 = (**(code **)(*piVar4 + 0x118))();
      if (iVar5 != -1) {
        piVar4 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x60),
                                   iVar5);
        (**(code **)(*piVar4 + 0x2c))();
      }
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  pcVar1 = *(code **)(param_1 + 0x78);
  uVar2 = *(undefined4 *)(param_1 + 0x7c);
  piVar4 = *(int **)(param_1 + 0x80);
  *(undefined4 *)(param_1 + 0x78) = 0;
  *(undefined4 *)(param_1 + 0x7c) = 0;
  *(undefined4 *)(param_1 + 0x80) = 0;
  uStack_16 = (short)((uint)pcVar1 >> 0x10);
  if (uStack_16 != 0) {
    (*pcVar1)(uVar2,piVar4);
  }
  if (piVar4 != (int *)0x0) {
    (**(code **)(*piVar4 + 0x2c))();
  }
  return;
}

