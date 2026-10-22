
void FUN_00448eb4(int param_1)

{
  int *piVar1;
  code *pcVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  pcVar2 = AoWE_AoWHSMap_6B957A58_exref;
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_0044921f;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 0xc4),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x17c),local_8);
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  iVar4 = *(int *)(iVar4 + 0xd0);
  AoWE_TPlayerList_GetPlayers_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  iVar5 = AoWE_TPlayer_GetUpkeep_23EDC2EF();
  SysUtils_IntToStr_0F6FDFF6(iVar4 - iVar5,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x170),local_8);
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 0xd0),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x150),local_8);
  AoWE_TPlayerList_GetPlayers_23EDC2EF
            (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  uVar6 = AoWE_TPlayer_GetUpkeep_23EDC2EF();
  SysUtils_IntToStr_0F6FDFF6(uVar6,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x168),local_8);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x140) + 0x118) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x184) + 0x118) + 0x40))();
  (**(code **)(**(int **)(*(int *)(param_1 + 0x188) + 0x118) + 0x40))();
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  iVar4 = *(int *)(iVar4 + 0xb0);
  if (iVar4 != 0) {
    System_LoadResString_02614E1B(AoWE_ExternalIncomeRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x184) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  }
  iVar4 = City_CityIncome_82863DB4
                    (CONCAT31((int3)((uint)*(int *)pcVar2 >> 8),
                              *(undefined1 *)(*(int *)pcVar2 + 0xa5)));
  if (iVar4 != 0) {
    System_LoadResString_02614E1B(AoWE_CitiesRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x184) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  }
  iVar4 = Farm_FarmIncome_82863DB4
                    (CONCAT31((int3)((uint)*(int *)pcVar2 >> 8),
                              *(undefined1 *)(*(int *)pcVar2 + 0xa5)));
  if (iVar4 != 0) {
    System_LoadResString_02614E1B(AoWE_FarmsRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x184) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  }
  iVar4 = Mine_MineIncome_82863DB4
                    (CONCAT31((int3)((uint)*(int *)pcVar2 >> 8),
                              *(undefined1 *)(*(int *)pcVar2 + 0xa5)));
  if (iVar4 != 0) {
    System_LoadResString_02614E1B(AoWE_MinesRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x184) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  }
  iVar4 = BldGuild_BuildersGuildIncome_82863DB4
                    (CONCAT31((int3)((uint)*(int *)pcVar2 >> 8),
                              *(undefined1 *)(*(int *)pcVar2 + 0xa5)));
  if (iVar4 != 0) {
    System_LoadResString_02614E1B(AoWE_BuildersGuildsRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    SysUtils_IntToStr_0F6FDFF6(iVar4,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x184) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  }
  iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)pcVar2 + 0x140),(int)*(char *)(*(int *)pcVar2 + 0xa5));
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 200),&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x188) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
  puVar3 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00449226;
  puStack_20 = (undefined1 *)0x44921e;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar3);
  return;
}

