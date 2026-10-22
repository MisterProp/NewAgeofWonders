
void FUN_0040cd08(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar4;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0040cdc2;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  iVar1 = *(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70);
  FUN_0040cb38(param_1);
  uVar4 = *(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c);
  iVar3 = AoWE_TAoWCampaignScenarioList_GetScenario_23EDC2EF
                    (*(undefined4 *)(iVar1 + 0x10),*(undefined4 *)(iVar1 + 0x28));
  uVar2 = *(undefined4 *)(iVar3 + 0x10);
  System__LStrCatN_51F89FF7(&local_8,3);
  AoWE_TAoWCampaignTransferSettings_SetupSelection_23EDC2EF(*(undefined4 *)(iVar1 + 0x20),local_8);
  if (*(int *)(*(int *)(iVar1 + 0x20) + 0x40) < 1) {
    FUN_0040cdec(param_1,param_1);
  }
  else {
    AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF
              (*(undefined4 *)(param_1 + 0x138),*(undefined4 *)(*(int *)(iVar1 + 0x20) + 0x44));
    AoWVScroll_TAOWScrollBar_SetFMax_23EDC2EF
              (*(undefined4 *)(param_1 + 0x13c),*(undefined4 *)(*(int *)(iVar1 + 0x20) + 0x48));
    FUN_0040c8d4(param_1);
    (**(code **)(**(int **)(param_1 + 0x80) + 0x6c))(*(int **)(param_1 + 0x80),1);
  }
  *in_FS_OFFSET = uVar2;
  System__LStrClr_40929B27(&local_8,uVar2,uVar4,&LAB_0040cdc9);
  return;
}

