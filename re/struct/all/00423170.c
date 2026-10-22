
void FUN_00423170(void)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_1c = &LAB_004232e9;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  puVar1 = &stack0xfffffffc;
  if (-1 < *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
    uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    AoWE_TPlayer_GetName_23EDC2EF(uVar2,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b134 + 0x88),local_8);
    if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) != '\0') {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_RaceNamesPluralRStr_B11890CE_exref + *(char *)(iVar3 + 0xa5) * 4),&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b134 + 0x8c),local_8);
    }
    System_LoadResString_02614E1B(AoWE_DayRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    SysUtils_IntToStr_0F6FDFF6
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x174),&local_10);
    uVar2 = local_10;
    System__LStrCatN_51F89FF7(&local_8,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(DAT_0045b134 + 0x5c),local_8);
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    iVar3 = (int)*(char *)(iVar3 + 0xa5);
    if (iVar3 == -1) {
      iVar3 = 0xc;
    }
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(DAT_0045b134 + 0x60),iVar3 + 0xd);
    iVar3 = **(int **)(DAT_0045b134 + 0x44);
    (**(code **)(iVar3 + 0x6c))(*(int **)(DAT_0045b134 + 0x44),1,iVar3,uVar2);
    puVar1 = puStack_18;
  }
  puStack_18 = puVar1;
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_004232f0;
  puStack_1c = (undefined1 *)0x4232e8;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar1);
  return;
}

