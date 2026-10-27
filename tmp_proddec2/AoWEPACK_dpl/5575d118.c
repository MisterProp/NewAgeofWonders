// /AoWEPACK.dpl : 5575d118

void AoWE_TProductionControl_ExecuteProduction_23EDC2EF(int *param_1,int param_2)

{
  undefined1 *puVar1;
  char cVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
                    /* 0x5d118  2358  AoWE.TProductionControl.ExecuteProduction@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_5575d314;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  cVar2 = (**(code **)(*param_1 + 0xb0))(param_1,param_2,&local_8);
  if (cVar2 == '\0') {
    *(undefined1 *)(param_1 + 8) = 1;
  }
  else {
    AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
    (**(code **)(*param_1 + 0x54))();
    *(undefined1 *)(param_1 + 8) = 1;
    if (*(int *)(param_2 + 8) == 1) {
      (**(code **)(*(int *)param_1[9] + 0x30))();
      *(undefined4 *)(param_1[9] + 0x28) = *(undefined4 *)(param_2 + 0xc);
      uVar4 = (**(code **)(*param_1 + 0x9c))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined4 *)(param_1[9] + 0x24) = uVar4;
      uVar4 = (**(code **)(*param_1 + 0xc4))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined4 *)(param_1[9] + 8) = uVar4;
      uVar4 = (**(code **)(*param_1 + 200))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined4 *)(param_1[9] + 0xc) = uVar4;
      uVar4 = (**(code **)(*param_1 + 0xcc))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined4 *)(param_1[9] + 0x14) = uVar4;
      uVar4 = (**(code **)(*param_1 + 0xd0))(param_1,*(undefined4 *)(param_2 + 0xc));
      iVar5 = param_1[9];
      *(undefined4 *)(iVar5 + 0x18) = uVar4;
      *(undefined4 *)(iVar5 + 0x20) = *(undefined4 *)(param_2 + 0x10);
      uVar3 = (**(code **)(*param_1 + 0xd4))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined1 *)(param_1[9] + 0x1c) = uVar3;
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      iVar5 = param_1[9];
      *(undefined1 *)(iVar5 + 0x1d) = uVar3;
      *(undefined1 *)(iVar5 + 0x10) = *(undefined1 *)(iVar5 + 0x1c);
      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),
                         (int)*(char *)(iVar5 + 0x1d));
      AoWE_TPlayer_SetGems_23EDC2EF(iVar5,*(int *)(iVar5 + 0xc4) - *(int *)(param_1[9] + 8));
      AoWE_TPlayerMagicControl_SetMana_23EDC2EF
                (*(int *)(iVar5 + 0x54),
                 *(int *)(*(int *)(iVar5 + 0x54) + 0x24) - *(int *)(param_1[9] + 0xc));
      *(undefined1 *)(param_1 + 8) = 2;
      if (*(int *)(param_1[9] + 0x20) != 0) {
        AoWE_TProductionControl_RegisterGroupProduction_23EDC2EF(param_1);
      }
    }
    else if (*(int *)(param_2 + 8) == 2) {
      (**(code **)(*(int *)param_1[9] + 0x30))();
      *(undefined4 *)(param_1[9] + 0x28) = *(undefined4 *)(param_2 + 0xc);
      uVar4 = (**(code **)(*param_1 + 0xe4))(param_1,*(undefined4 *)(param_2 + 0xc));
      iVar5 = param_1[9];
      *(undefined4 *)(iVar5 + 8) = uVar4;
      *(undefined4 *)(iVar5 + 0xc) = 0;
      uVar3 = (**(code **)(*param_1 + 0xec))(param_1,*(undefined4 *)(param_2 + 0xc));
      *(undefined1 *)(param_1[9] + 0x1c) = uVar3;
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      iVar5 = param_1[9];
      *(undefined1 *)(iVar5 + 0x1d) = uVar3;
      *(undefined1 *)(iVar5 + 0x10) = *(undefined1 *)(iVar5 + 0x1c);
      iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),
                         (int)*(char *)(iVar5 + 0x1d));
      AoWE_TPlayer_SetGems_23EDC2EF(iVar5,*(int *)(iVar5 + 0xc4) - *(int *)(param_1[9] + 8));
      AoWE_TPlayerMagicControl_SetMana_23EDC2EF
                (*(int *)(iVar5 + 0x54),
                 *(int *)(*(int *)(iVar5 + 0x54) + 0x24) - *(int *)(param_1[9] + 0xc));
      *(undefined1 *)(param_1 + 8) = 2;
    }
    AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_5575d31b;
  puStack_1c = (undefined1 *)0x5575d313;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar1);
  return;
}

