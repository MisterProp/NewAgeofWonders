
void FUN_00452ac8(int param_1,int param_2,int param_3,undefined4 param_4)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  
  if (param_2 != -1) {
    if ((*(int *)AoWE_AoWHSMap_6B957A58_exref == 0) ||
       ((*(byte *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x3c) & 4) == 0)) {
      if ((*(short *)(param_1 + 0x1e8) != 1) &&
         (cVar1 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF
                            (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x50),
                             (int)*(short *)(param_1 + 0x1e8)), cVar1 != '\0')) {
        uVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x50),
                           (int)*(short *)(param_1 + 0x1e8));
        ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar4,param_2 + -0x19,param_3 + -0x19,param_4);
      }
    }
    else {
      piVar5 = *(int **)(param_1 + 0x1f8);
      if (piVar5 != (int *)0x0) {
        (**(code **)(*piVar5 + 0x84))(piVar5,param_2 + -0xf,param_3 + -0x14,param_4);
      }
      iVar7 = (int)*(short *)(param_1 + 0x1e8);
      if ((iVar7 == 9999) && (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0)) {
        iVar7 = (**(code **)**(undefined4 **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc))();
      }
      if ((iVar7 != 1) &&
         (iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x4c),iVar7),
         iVar2 != 0)) {
        if ((*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) != -1) &&
           (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x11c) == 0)) {
          AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
          cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
          if ((cVar1 != '\0') &&
             (iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                                 (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)),
             *(char *)(iVar2 + 0x44) == '\0')) {
            uVar3 = (*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x40) << 2) % 100 - 0x32;
            uVar6 = (int)uVar3 >> 0x1f;
            uVar4 = GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                              (*(undefined4 *)GFXE_AlphaBlendTable_AB1D79F5_exref,
                               ((uVar3 ^ uVar6) - uVar6) + 0x1e,100,param_4);
            piVar5 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x4c),
                                       iVar7);
            (**(code **)(*piVar5 + 0x94))(piVar5,param_2 + -0x19,param_3 + -0x19,uVar4);
            *(undefined1 *)(param_1 + 0x20c) = 1;
            return;
          }
        }
        piVar5 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x4c),
                                   iVar7);
        (**(code **)(*piVar5 + 0x84))(piVar5,param_2 + -0x19,param_3 + -0x19,param_4);
      }
    }
  }
  return;
}

