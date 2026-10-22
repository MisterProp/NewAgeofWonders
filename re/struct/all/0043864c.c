
void FUN_0043864c(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
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
  puStack_28 = &LAB_0043876c;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  cVar3 = FUN_00438628(param_1);
  if (cVar3 != '\0') {
    (**(code **)(**(int **)(param_1 + 0xa4) + 0x30))();
    uVar4 = AoWE_TUnitControl_FindUnit_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xfc),
                       *(undefined4 *)(*(int *)(param_1 + 0x9c) + 0x18));
    *(undefined4 *)(param_1 + 0xa8) = uVar4;
    (**(code **)(**(int **)(param_1 + 0x9c) + 0x7c))
              (*(int **)(param_1 + 0x9c),*(undefined4 *)(param_1 + 0xa4),param_1 + 0xa1);
    puVar5 = &local_8;
    cVar3 = AoWE_THero_GetLevel_23EDC2EF(*(undefined4 *)(param_1 + 0xa8));
    SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_LevelXRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x7c),*(undefined4 *)(*(int *)(param_1 + 0xa8) + 0x60));
    piVar1 = *(int **)(*(int *)(param_1 + 0x9c) + 0x2c);
    (**(code **)(*piVar1 + 0x1c))(piVar1,&local_8);
    piVar1 = *(int **)(*(int *)(param_1 + 0x8c) + 0x118);
    (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  }
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_00438773;
  puStack_28 = (undefined1 *)0x438763;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar2);
  puStack_28 = (undefined1 *)0x43876b;
  System__LStrClr_40929B27(&local_8);
  return;
}

