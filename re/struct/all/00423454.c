
void FUN_00423454(int param_1,undefined4 param_2,int *param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  uint uVar9;
  
  piVar4 = *(int **)(*(int *)(param_1 + 0x54) + 0x114);
  uVar2 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  piVar4 = (int *)EventLog_TEventLogbook_GetLogs_23EDC2EF(*(undefined4 *)(iVar3 + 0xd8),uVar2);
  iVar3 = (**(code **)(*piVar4 + 0x54))();
  piVar4 = *(int **)(*(int *)(param_1 + 0x54) + 0x114);
  uVar2 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
  iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  piVar4 = (int *)EventLog_TEventLogbook_GetLogs_23EDC2EF(*(undefined4 *)(iVar5 + 0xd8),uVar2);
  iVar5 = (**(code **)(*piVar4 + 0x58))();
  piVar4 = *(int **)(*(int *)(param_1 + 0x54) + 0x114);
  uVar2 = (**(code **)(*piVar4 + 0x18))(piVar4,param_5);
  iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  piVar4 = (int *)EventLog_TEventLogbook_GetLogs_23EDC2EF(*(undefined4 *)(iVar6 + 0xd8),uVar2);
  iVar6 = (**(code **)(*piVar4 + 0x5c))();
  if (-1 < iVar3) {
    if (iVar5 == -1) {
      uVar2 = param_4;
      iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xcc),iVar3);
      uVar9 = 0x24 - *(int *)(iVar7 + 0x20);
      iVar7 = (int)uVar9 >> 1;
      if (iVar7 < 0) {
        iVar7 = iVar7 + (uint)((uVar9 & 1) != 0);
      }
      iVar1 = *param_3;
      uVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xcc),iVar3);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar8,iVar7 + iVar1,param_3[1] + 1,uVar2);
    }
    else {
      uVar2 = param_4;
      uVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xcc),iVar3);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar8,*param_3,param_3[1] + 1,uVar2);
    }
  }
  if (-1 < iVar5) {
    uVar2 = param_4;
    uVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xcc),iVar5);
    ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar8,*param_3 + 0xc,param_3[1] + 1,uVar2);
  }
  if (-1 < iVar6) {
    uVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xcc),iVar6);
    ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar2,*param_3 + 0x18,param_3[1] + 1,param_4);
  }
  return;
}

