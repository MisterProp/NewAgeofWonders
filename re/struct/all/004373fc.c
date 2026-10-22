
void FUN_004373fc(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00437505;
  puStack_1c = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = &puStack_1c;
  *(undefined4 *)(param_1 + 0x84) = param_4;
  *(undefined4 *)(param_1 + 0x88) = param_5;
  *(int **)(param_1 + 0x8c) = param_2;
  (**(code **)(*param_2 + 0x28))();
  iVar4 = *(int *)(param_1 + 0x8c);
  (**(code **)(**(int **)(iVar4 + 0x2c) + 0x1c))(*(int **)(iVar4 + 0x2c),&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(iVar4 + 0x38));
  uVar3 = HSEngine_TMapContainer_GetField_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x10),
                     (int)*(char *)(iVar4 + 0x3c),(int)*(char *)(iVar4 + 0x3d),
                     (int)*(char *)(iVar4 + 0x3e));
  iVar4 = HSEngine_TMapField_FindNoneTransparentHS_23EDC2EF(uVar3,City__TCity_01CDE5D7_exref);
  *(undefined1 *)(param_1 + 0x80) = *(undefined1 *)(iVar4 + 0x45);
  uVar3 = City_TCity_GetTopCityActionImage_23EDC2EF(iVar4,0);
  AoWArnoLine_TAoWArnoLine_SetITop_23EDC2EF(*(undefined4 *)(param_1 + 0x74),uVar3);
  uVar3 = City_TCity_GetMiddleCityActionImage_23EDC2EF(iVar4,0);
  AoWArnoLine_TAoWArnoLine_SetIMiddle_23EDC2EF(*(undefined4 *)(param_1 + 0x74),uVar3);
  uVar3 = City_TCity_GetBottomCityActionImage_23EDC2EF(iVar4,0);
  AoWArnoLine_TAoWArnoLine_SetIBottom_23EDC2EF(*(undefined4 *)(param_1 + 0x74),uVar3);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  AoWE_TTutorialControl_ShowMessage_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x3e,1);
  puVar2 = puStack_1c;
  *in_FS_OFFSET = 1;
  puStack_1c = &LAB_0043750c;
  System__LStrClr_40929B27(&local_8,1,puVar2);
  return;
}

