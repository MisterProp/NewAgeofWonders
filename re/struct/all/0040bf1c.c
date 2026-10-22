
void FUN_0040bf1c(int *param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  if ((-1 < param_2) && (param_2 < 3)) {
    param_1[0xe] = 2;
    param_1[0xf] = 0;
    iVar1 = FUN_0040bde4(param_1,param_1[9]);
    uVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar1 + 0x2c),param_2);
    uVar2 = AoWE_TAoWCampaignScenarioList_IndexOfID_23EDC2EF(param_1[4],uVar2);
    (**(code **)(*param_1 + 0x54))(param_1,uVar2);
    iVar1 = FUN_0040bde4(param_1,param_1[10]);
    iVar1 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar1 + 0x2c),3);
    if (iVar1 == -1) {
      iVar1 = FUN_0040bde4(param_1,param_1[9]);
      uVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar1 + 0x2c),3);
      iVar1 = FUN_0040bde4(param_1,param_1[10]);
      Engine_TIntegerList_Put_23EDC2EF(*(undefined4 *)(iVar1 + 0x2c),3,uVar2);
    }
  }
  return;
}

