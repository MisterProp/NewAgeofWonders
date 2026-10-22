
void FUN_00428894(int param_1,char param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_18;
  int *local_14;
  undefined4 local_10;
  char local_9;
  int local_8;
  
  puStack_28 = &stack0xfffffffc;
  local_18 = 0;
  puStack_2c = &LAB_004289a9;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  local_10 = param_3;
  local_9 = param_2;
  local_8 = param_1;
  uVar2 = System_TObject_Create_23EDC2EF(Engine__TIntegerList_D8218C48_exref,1);
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)local_9);
  local_14 = *(int **)(*(int *)(iVar3 + 0x60) + 8);
  iVar3 = (**(code **)(*local_14 + 0x54))();
  if (-1 < iVar3 + -1) {
    iVar6 = 0;
    do {
      iVar4 = AoWE_TTurnInfo_GetTurnInfo_23EDC2EF(local_14,iVar6);
      Engine_TIntegerList_Add_23EDC2EF(uVar2,*(undefined4 *)(iVar4 + 0x14));
      iVar6 = iVar6 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  uVar9 = *(undefined4 *)(local_8 + 0xe0);
  uVar8 = *(undefined4 *)(local_8 + 0xec);
  uVar7 = local_10;
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)local_9);
  uVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xe4),*(char *)(iVar3 + 0xa5) + 0xd)
  ;
  FUN_004283b8(local_8,uVar5,uVar2,param_4,uVar7,uVar8,uVar9);
  System_TObject_Free_23EDC2EF(uVar2);
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(local_8 + 0xec),&local_18);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xcc),local_18);
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_004289b0;
  puStack_2c = (undefined1 *)0x4289a8;
  System__LStrClr_40929B27(&local_18,uStack_30,puVar1);
  return;
}

