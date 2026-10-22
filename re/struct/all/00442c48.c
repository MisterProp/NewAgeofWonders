
void FUN_00442c48(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_1c = &LAB_00442cf9;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xac) + 0x6c))(*(int **)(param_1 + 0xac),1);
  uVar3 = (**(code **)(*param_2 + 0x144))();
  piVar1 = *(int **)(*(int *)(param_1 + 0xb0) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,uVar3);
  uVar3 = WizardsTower_TWizardsTower_CanEnter_23EDC2EF
                    (param_2,*(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),uVar3);
  System_LoadResString_02614E1B(AoWE_EnterRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_8);
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00442d00;
  puStack_1c = (undefined1 *)0x442cf8;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

