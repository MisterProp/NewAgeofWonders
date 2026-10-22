
void FUN_00447f5c(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar5;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_28 = &LAB_0044809b;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  puVar2 = &stack0xfffffffc;
  if (*(int *)(param_1 + 0x94) != 0) {
    System__Assert_E17CE8DC("Assertion failure","D:\\AoWDev\\AoWHeroInfoDlg.pas",0x50);
    puVar2 = puStack_24;
  }
  puStack_24 = puVar2;
  *(undefined4 *)(param_1 + 0x8c) = param_4;
  *(undefined4 *)(param_1 + 0x90) = param_5;
  *(int **)(param_1 + 0x94) = param_2;
  (**(code **)(*param_2 + 0x28))();
  (**(code **)(*(int *)param_2[8] + 0x1c))((int *)param_2[8],&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x84) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  iVar4 = AoWE_TArmy_Hero_23EDC2EF
                    (*(undefined4 *)(*(int *)(param_1 + 0x94) + 0x18),
                     *(undefined1 *)(*(int *)(param_1 + 0x94) + 0x1c),*AoWE_htAll_E90514D2_exref);
  if (iVar4 == 0) {
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),0);
  }
  else {
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x74),*(undefined4 *)(iVar4 + 0x60));
    puVar5 = &local_8;
    cVar3 = AoWE_THero_GetLevel_23EDC2EF(iVar4);
    SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_LevelXRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
  }
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),1);
  puVar2 = puStack_24;
  *(undefined1 *)(param_1 + 0x98) = 0;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_004480a2;
  puStack_28 = (undefined1 *)0x448092;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar2);
  puStack_28 = (undefined1 *)0x44809a;
  System__LStrClr_40929B27(&local_8);
  return;
}

