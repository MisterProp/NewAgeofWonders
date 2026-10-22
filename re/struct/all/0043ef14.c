
void FUN_0043ef14(int param_1)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  
  iVar3 = *(int *)(*(int *)(param_1 + 0x1fc) + 8) + -1;
  if (-1 < iVar3) {
    do {
      uVar1 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x1fc),iVar3);
      piVar2 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                                (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x60),uVar1)
      ;
      (**(code **)(*piVar2 + 0x2c))();
      iVar3 = iVar3 + -1;
    } while (iVar3 != -1);
  }
  Engine_TIntegerList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x1fc));
  return;
}

