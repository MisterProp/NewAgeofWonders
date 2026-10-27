// /AoWEPACK.dpl : 557ab0d4

void City_TCity_MigrationTime_23EDC2EF(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_44;
  undefined1 *puStack_40;
  undefined1 *puStack_3c;
  undefined1 local_2c [12];
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_c;
  char local_5;
  
                    /* 0xab0d4  6885  City.TCity.MigrationTime@23EDC2EF */
  puStack_3c = &stack0xfffffffc;
  local_c = 0;
  local_5 = (char)param_2;
  puStack_40 = &LAB_557ab1c0;
  uStack_44 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_44;
  cVar3 = City_TCity_CanMigrate_23EDC2EF(param_1,param_2,&local_c);
  if ((cVar3 != '\0') && (local_5 != *(char *)(param_1 + 0x45))) {
    iVar6 = -1;
    iVar1 = *(int *)(*(int *)(param_1 + 4) + 4);
    HSEngine_InitHNWalk_3DC7E9B4
              (local_2c,(int)*(char *)(param_1 + 0x10),(int)*(char *)(param_1 + 0x11),1);
    while ((local_20 < 0x15 && (iVar6 == -1))) {
      if ((-1 < local_1c) &&
         (((-1 < local_18 && (local_1c < *(int *)(iVar1 + 0xc))) &&
          (local_18 < *(int *)(iVar1 + 0x10))))) {
        uVar4 = HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,local_1c,local_18);
        iVar5 = HSEngine_TMapField_FindNoneTransparentHS_23EDC2EF(uVar4,&City__TCity_01CDE5D7);
        if ((iVar5 != 0) && (*(char *)(iVar5 + 0x45) == local_5)) {
          iVar6 = local_20;
        }
      }
      HSEngine_WalkToNextHN_77BE26A7(local_2c);
    }
  }
  puVar2 = puStack_3c;
  *in_FS_OFFSET = uStack_44;
  puStack_3c = &LAB_557ab1c7;
  puStack_40 = (undefined1 *)0x557ab1bf;
  System__LStrClr_40929B27(&local_c,uStack_44,puVar2);
  return;
}

