
void FUN_00443078(int param_1,int *param_2)

{
  int iVar1;
  undefined1 *puVar2;
  char cVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_28 = &LAB_004431fd;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xb4) + 0x6c))(*(int **)(param_1 + 0xb4),1);
  cVar3 = Altar_TAltar_GetTargeting_23EDC2EF(param_2);
  if (cVar3 == '\0') {
    System_LoadResString_02614E1B(AoWE_TargetRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_8);
  }
  else {
    System_LoadResString_02614E1B(AoWE_CancelRStr_8276FCC3_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_8);
  }
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
  uVar4 = (**(code **)(*param_2 + 0x22c))();
  iVar1 = *(int *)(param_1 + 0xb8);
  *(uint *)(iVar1 + 0x118) = uVar4 & 0xff;
  AOWGauge_TAoWGauge_SetPos_23EDC2EF(iVar1,*(undefined1 *)((int)param_2 + 0x41));
  (**(code **)(*param_2 + 0x22c))();
  uVar5 = System__ROUND_51F89FF7();
  SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
  System__LStrCat_51F89FF7(&local_8,&DAT_00443214);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xc4),local_8);
  uVar4 = (**(code **)(*param_2 + 0x22c))();
  SysUtils_IntToStr_0F6FDFF6((uVar4 & 0xff) - (uint)*(byte *)((int)param_2 + 0x41),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd0),local_8);
  uVar5 = (**(code **)(*param_2 + 0x23c))();
  SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xc0),local_8);
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),1);
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_00443204;
  puStack_28 = (undefined1 *)0x4431fc;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

