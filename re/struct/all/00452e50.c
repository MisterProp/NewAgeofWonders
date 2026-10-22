
void FUN_00452e50(int param_1,undefined4 param_2,int *param_3)

{
  undefined1 *puVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  int *piVar7;
  undefined4 *in_FS_OFFSET;
  byte bVar8;
  undefined4 uStackY_88;
  undefined1 *puStackY_84;
  undefined4 uStack_7c;
  undefined1 *puStack_78;
  undefined1 *puStack_74;
  undefined4 local_64;
  undefined1 local_60 [60];
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_14;
  int local_10;
  int *local_c;
  int local_8;
  
  bVar8 = 0;
  puStack_74 = &stack0xfffffffc;
  local_64 = 0;
  puStack_78 = &LAB_00453118;
  uStack_7c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_7c;
  local_c = param_3;
  local_8 = param_1;
  cVar2 = AoWReg_TAoWRegistry_GetDebugMode_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c));
  if (cVar2 == '\0') {
    puStackY_84 = (undefined1 *)0x452e95;
    Sleep(0);
  }
  else {
    puStackY_84 = (undefined1 *)0x452e8c;
    Sleep(1);
  }
  if (DAT_0045b2b5 == '\x01') {
    DAT_0045b2b5 = '\0';
    AoWSetup_TSetupControl_hPlayerSetupDone_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a30c + 0x33c));
  }
  else if (DAT_0045b2b5 == '\x02') {
    DAT_0045b2b5 = '\0';
    FUN_0041fc08(*(undefined4 *)PTR_DAT_0045a4c8);
  }
  if (*(int *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1b0) != 0) {
    aowDPlayComp_TGDPlay_CheckMessages_23EDC2EF
              (*(int *)(*(int *)PTR_DAT_0045a454 + 0x44),PTR_DAT_0045a454);
  }
  Sound_ProcessSound_51F89FF7();
  puStackY_84 = &LAB_00452f26;
  uStackY_88 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStackY_88;
  if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
    (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xe8))();
  }
  *in_FS_OFFSET = uStackY_88;
  if ((local_c != (int *)0x0) && (iVar4 = (**(code **)(*local_c + 0x40))(local_c,1), iVar4 != 0)) {
    (**(code **)(**(int **)(local_8 + 0x1dc) + 0xa8))
              (*(int **)(local_8 + 0x1dc),&local_10,&local_14);
    puStackY_84 = (undefined1 *)0x452f82;
    FUN_00452918(local_8,local_10,local_14);
    if (((*(char *)(local_8 + 0x20c) == '\0') &&
        ((((*(int *)AoWE_AoWHSMap_6B957A58_exref == 0 ||
           (*(char *)(*(int *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc) + 0x3a) == '\0')) &&
          (*(int *)(local_8 + 0x1fc) == local_24)) &&
         ((*(int *)(local_8 + 0x200) == local_20 && (*(int *)(local_8 + 0x204) == local_1c)))))) &&
       (*(int *)(local_8 + 0x208) == local_18)) {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
    *(undefined1 *)(local_8 + 0x20c) = uVar3;
    if (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) {
      FUN_00452cc0(local_8,local_10,local_14);
    }
    puStackY_84 = &LAB_0045303d;
    uStackY_88 = *in_FS_OFFSET;
    *in_FS_OFFSET = &uStackY_88;
    uVar5 = (**(code **)(*local_c + 0x40))(local_c,1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0x44) + 0x78))
              (*(int **)(*(int *)PTR_DAT_0045a420 + 0x44),uVar5);
    *in_FS_OFFSET = uStackY_88;
    if (local_10 != -1) {
      piVar6 = (int *)(**(code **)(*local_c + 0x40))(local_c,1);
      iVar4 = (**(code **)(*piVar6 + 0x2c))(piVar6,local_60);
      if (-1 < iVar4) {
        puStackY_84 = (undefined1 *)0x453082;
        FUN_00452ac8(local_8,local_10,local_14);
        piVar6 = (int *)(**(code **)(*local_c + 0x40))(local_c,1);
        (**(code **)(*piVar6 + 0x30))();
      }
    }
    piVar6 = &local_24;
    piVar7 = (int *)(local_8 + 0x1fc);
    for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
      *piVar7 = *piVar6;
      piVar6 = piVar6 + (uint)bVar8 * -2 + 1;
      piVar7 = piVar7 + (uint)bVar8 * -2 + 1;
    }
    SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(local_8 + 0x1dc) + 0x17c),&local_64);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a5fc + 0x60),local_64);
  }
  if ((*(int *)AoWE_AoWHSMap_6B957A58_exref != 0) &&
     (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x1c0) != '\0')) {
    FUN_0045530c(*(undefined4 *)PTR_DAT_0045a420);
  }
  FUN_0040b390(*(undefined4 *)PTR_DAT_0045a478);
  puVar1 = puStack_74;
  *in_FS_OFFSET = uStack_7c;
  puStack_74 = &LAB_0045311f;
  puStack_78 = (undefined1 *)0x453117;
  System__LStrClr_40929B27(&local_64,uStack_7c,puVar1);
  return;
}

