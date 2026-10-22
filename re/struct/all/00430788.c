
void FUN_00430788(int param_1,int param_2)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0043093b;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  cVar2 = FUN_0042eecc(param_1);
  if (cVar2 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
  }
  else {
    cVar2 = *(char *)(param_1 + 0x220);
    if (cVar2 == '\0') {
      cVar2 = (**(code **)(**(int **)(param_1 + 0x22c) + 300))();
      if ((int)cVar2 < *(int *)(param_2 + 0x14)) {
        iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(*(int *)(param_1 + 0x22c) + 0x24));
        cVar2 = AoWE_TPlayerMagicControl_GetCheatEnabled_23EDC2EF(*(undefined4 *)(iVar3 + 0x54));
        if (cVar2 == '\0') {
          cVar2 = AoWE_THero_CastSpell_23EDC2EF
                            (*(undefined4 *)(param_1 + 0x22c),*(undefined4 *)(param_2 + 0x10),
                             &local_8);
          if (cVar2 == '\0') {
            (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                      (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
          }
          else {
            FUN_0043073c(param_1,param_1);
          }
          goto LAB_00430925;
        }
      }
      cVar2 = AoWE_THero_CastSpell_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x22c),*(undefined4 *)(param_2 + 0x10),&local_8);
      if (cVar2 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
      }
    }
    else if (cVar2 == '\x01') {
      cVar2 = AoWE_THero_CastCombatSpell_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x22c),*(undefined4 *)(param_2 + 0x10),&local_8);
      if (cVar2 == '\0') {
        (**(code **)(**(int **)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0x114))
                  (*(int **)AoWTC_AoWCombatMap_2AEF5FAB_exref,local_8);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
      }
    }
    else if ((byte)(cVar2 - 2U) < 2) {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      cVar2 = AoWE_TPlayerMagicControl_ResearchSpell_23EDC2EF
                        (*(undefined4 *)(iVar3 + 0x54),*(undefined4 *)(param_2 + 0x10));
      if (cVar2 != '\0') {
        if ((*(int *)(param_1 + 0x240) == 0) ||
           (*(char *)(*(int *)(param_1 + 0x240) + 0x18) != '\x02')) {
          *(undefined1 *)(param_1 + 0x220) = 2;
          FUN_0043073c(param_1,param_1);
        }
        else {
          (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
        }
      }
    }
  }
LAB_00430925:
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00430942;
  puStack_18 = (undefined1 *)0x43093a;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

