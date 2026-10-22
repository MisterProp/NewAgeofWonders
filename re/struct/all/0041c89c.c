
void FUN_0041c89c(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  int *piVar3;
  int *in_FS_OFFSET;
  int iStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined1 local_10;
  undefined4 local_c;
  uint local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_c = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_0041ca48;
  iStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_34;
  (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),0);
  iVar2 = FUN_0041bbb8(param_1);
  local_8 = (uint)*(byte *)(*(int *)(*(int *)(iVar2 + 0x20) + 8) + 0x9c);
  piVar3 = (int *)AoWE_TLeaderSetupControl_Create_23EDC2EF
                            (AoWE__TLeaderSetupControl_4BFAB016_exref,1,0);
  iVar2 = FUN_0041bbb8(param_1);
  (**(code **)(*(int *)piVar3[3] + 0x3c))
            ((int *)piVar3[3],*(undefined4 *)(*(int *)(iVar2 + 0x20) + 8),
             *(undefined4 *)AoWE_AoWEngine_7E705837_exref);
  AoWE_TLeader_SetSpherePicksMax_23EDC2EF(piVar3[3],(char)local_8 + '\x02');
  (**(code **)(*(int *)piVar3[5] + 0x3c))
            ((int *)piVar3[5],piVar3[3],*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
  *(undefined1 *)(piVar3 + 4) = 4;
  FUN_00416988(*(undefined4 *)PTR_DAT_0045a3f0,piVar3);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a3f0 + 0x58) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a3f0 + 0x58),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a3f0 + 0x54) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a3f0 + 0x54),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a3f0 + 0x5c) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a3f0 + 0x5c),0);
  FUN_00416898(*(undefined4 *)PTR_DAT_0045a3f0,local_8);
  SysUtils_IntToStr_0F6FDFF6(2,&local_18);
  local_14 = local_18;
  local_10 = 0xb;
  System_LoadResString_02614E1B(AoWE_AssignXNewSpheresRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_1c);
  AoWE_Format_D8F48273(local_1c,&local_14,0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a3f0 + 0x2c8),local_c);
  piVar3[9] = param_1;
  piVar3[8] = (int)&LAB_0041c814;
  (**(code **)(*piVar3 + 0x2c))();
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a3f0 + 0x2cc) + 0xa4))
            (*(int **)(*(int *)PTR_DAT_0045a3f0 + 0x2cc),
             *(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x9c));
  AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a3f0 + 0x2cc),0);
  puVar1 = puStack_30;
  *in_FS_OFFSET = (int)&local_c;
  puStack_30 = &LAB_0041ca4f;
  iStack_34 = 0x41ca3f;
  System__LStrArrayClr_51F89FF7(&local_1c,2,puVar1);
  iStack_34 = 0x41ca47;
  System__LStrClr_40929B27(&local_c);
  return;
}

