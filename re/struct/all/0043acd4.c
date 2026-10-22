
void FUN_0043acd4(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x5c),param_2);
  uVar1 = *(undefined4 *)(iVar2 + 0x18);
  piVar3 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x60),uVar1);
  (**(code **)(*piVar3 + 0x28))();
  Engine_TIntegerList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x2d8),uVar1);
  return;
}

