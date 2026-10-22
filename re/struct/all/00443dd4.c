
void FUN_00443dd4(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_00443f37;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0xd8) + 0x6c))(*(int **)(param_1 + 0xd8),1);
  cVar3 = System__IsClass_51F89FF7(param_2,City__TCity_01CDE5D7_exref);
  if ((cVar3 == '\0') || ((char)param_2[0xc] != '\0')) {
    uVar4 = (**(code **)(*param_2 + 0x144))();
    piVar1 = *(int **)(*(int *)(param_1 + 0xdc) + 0x118);
    (**(code **)(*piVar1 + 8))(piVar1,uVar4);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x1a4) + 0x6c))(*(int **)(param_1 + 0x1a4),1);
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (AoWE_RaceNamesPluralRStr_B11890CE_exref + *(char *)((int)param_2 + 0x45) * 4),
               &local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1a4),local_8);
    (**(code **)(**(int **)(param_1 + 0x1a8) + 0x6c))(*(int **)(param_1 + 0x1a8),1);
    (**(code **)(**(int **)(*(int *)(param_1 + 0xdc) + 0x118) + 0x40))();
  }
  if ((char)param_2[0xc] == -1) {
    (**(code **)(**(int **)(param_1 + 0xe4) + 0x6c))(*(int **)(param_1 + 0xe4),0);
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),0);
  }
  else {
    iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)(char)param_2[0xc]);
    iVar5 = (int)*(char *)(iVar5 + 0xa5);
    if (iVar5 == -1) {
      iVar5 = 0xc;
    }
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)(AoWE_RaceNamesPluralRStr_B11890CE_exref + iVar5 * 4),&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xe4),local_8);
    (**(code **)(**(int **)(param_1 + 0xe4) + 0x6c))(*(int **)(param_1 + 0xe4),1);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0xd4),iVar5 + 0x1a);
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),1);
  }
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00443f3e;
  puStack_1c = (undefined1 *)0x443f36;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar2);
  return;
}

