
void FUN_00413aac(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5)

{
  int *piVar1;
  undefined4 uVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar3;
  undefined *puVar4;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  puStack_24 = &LAB_00413ba8;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  local_8 = param_3;
  uVar2 = aowDPlayComp_TGDPlay_GetPlayerIndex_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x44),param_4,param_3,"Sending Map to ");
  piVar1 = *(int **)(*(int *)(param_1 + 0x44) + 0x1e8);
  (**(code **)(*piVar1 + 0xc))(piVar1,uVar2,&local_10);
  puVar4 = &DAT_00413bd8;
  uVar2 = local_10;
  System__LStrCatN_51F89FF7(&local_c,3);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x44),local_c);
  SysUtils_IntToStr_0F6FDFF6(param_5 / 1000,&local_10,1000,"0k of ",puVar4,uVar2);
  puVar4 = &DAT_00413bf4;
  uVar2 = local_10;
  System__LStrCatN_51F89FF7(&local_c,3);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x50),local_c);
  AOWGauge_TAoWGauge_SetPos_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a24c + 0x58),0);
  *(int *)(*(int *)(*(int *)PTR_DAT_0045a24c + 0x58) + 0x118) = param_5;
  uVar3 = 2;
  aowDPlayComp_TGDPlay_SendFileDataMessage_23EDC2EF
            (*(undefined4 *)(param_1 + 0x44),param_4,local_8,2,"MP Map",param_5,puVar4,uVar2);
  *in_FS_OFFSET = uVar3;
  System__LStrArrayClr_51F89FF7(&local_10,2,param_5,&LAB_00413baf);
  return;
}

