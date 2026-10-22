
void FUN_0041444c(void)

{
  int *piVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_004145aa;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  (**(code **)(**(int **)(*(int *)(*(int *)PTR_DAT_0045a490 + 0x50) + 0x114) + 0x40))();
  System_LoadResString_02614E1B(AoWE_AllPlayersRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a490 + 0x50) + 0x114);
  (**(code **)(*piVar1 + 0x38))(piVar1,local_8,0);
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    iVar4 = (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140) + 0x54))();
    iVar4 = iVar4 + -1;
    if (0 < iVar4) {
      iVar6 = 1;
      do {
        if (iVar6 != *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
          iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar6);
          if (*(char *)(iVar5 + 0x45) != '\x02') {
            iVar5 = NetworkE_TPlayerCommunicationControl_GetPlayers_23EDC2EF
                              (*(undefined4 *)
                                (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x240) + 0x1c),
                               iVar6);
            if (*(char *)(iVar5 + 8) == '\x01') {
              iVar5 = NetworkE_TPlayerCommunicationControl_GetPlayers_23EDC2EF
                                (*(undefined4 *)
                                  (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x240) + 0x1c),
                                 iVar6);
              uVar2 = *(undefined4 *)(iVar5 + 0xc);
              iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar6
                                );
              piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a490 + 0x50) + 0x114);
              (**(code **)(*piVar1 + 0x38))
                        (piVar1,*(undefined4 *)(*(int *)(iVar5 + 0xd4) + 0x60),uVar2);
              AoWListBox_TAOWListBox_SetIndex_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a490 + 0x50),0);
            }
          }
        }
        iVar6 = iVar6 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
  }
  puVar3 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_004145b1;
  puStack_20 = (undefined1 *)0x4145a9;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar3);
  return;
}

