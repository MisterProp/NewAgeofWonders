
void FUN_0041e1c0(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined1 uVar5;
  
  iVar2 = FUN_0041bbb8(param_1);
  uVar5 = *(int *)(iVar2 + 0x38) == 1;
  if ((bool)uVar5) {
    iVar2 = FUN_0041bbb8(param_1);
    uVar4 = *(undefined4 *)(iVar2 + 0x24);
    uVar3 = FUN_0041bbb8(param_1);
    iVar2 = FUN_0040bde4(uVar3,uVar4);
    uVar4 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar2 + 0x2c),1);
    iVar2 = FUN_0041bbb8(param_1);
    uVar4 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(*(undefined4 *)(iVar2 + 0x10),uVar4);
    uVar3 = FUN_0041bbb8(param_1);
    iVar2 = FUN_0040bde4(uVar3,uVar4);
    System__LStrCmp_51F89FF7
              (*(undefined4 *)(*(int *)(param_1 + 100) + 0x90),*(undefined4 *)(iVar2 + 0x18));
    if (!(bool)uVar5) {
      uVar3 = FUN_0041bbb8(param_1);
      iVar2 = FUN_0040bde4(uVar3,uVar4);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF
                (*(undefined4 *)(param_1 + 100),*(undefined4 *)(iVar2 + 0x18));
      piVar1 = *(int **)(param_1 + 100);
      (**(code **)(*piVar1 + 0x5c))(piVar1,0x1cc - piVar1[0x20]);
    }
  }
  *(undefined1 *)(param_1 + 0x158) = 1;
  return;
}

