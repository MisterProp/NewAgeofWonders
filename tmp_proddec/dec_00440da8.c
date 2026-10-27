
void MainBtn2Click(int param_1)

{
  uint uVar1;
  undefined1 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00440f20;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(char *)(param_1 + 500) == '\x01') {
    uVar1 = *(uint *)(param_1 + 0x1ec);
    if (uVar1 < 4) {
      iVar4 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),uVar1);
      if (*(char *)(iVar4 + 0xc) == '\0') {
        cVar3 = PrdPlace_TProductionPlace_CanResearch_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),1,
                           &local_8);
        if (cVar3 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          PrdPlace_TProductionPlace_Research_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),1);
        }
      }
      else {
        cVar3 = PrdPlace_TProductionPlace_CanProduce_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),1,
                           &local_8);
        if (cVar3 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          PrdPlace_TProductionPlace_Produce_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),1);
        }
      }
    }
    else {
      puStack_14 = &stack0xfffffffc;
      if (uVar1 == 4) {
        puStack_14 = &stack0xfffffffc;
        cVar3 = PrdPlace_TProductionPlace_CanTrade_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x1e8),1,&local_8);
        if (cVar3 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          PrdPlace_TProductionPlace_Trade_23EDC2EF(*(undefined4 *)(param_1 + 0x1e8),1);
        }
      }
    }
  }
  else {
    puStack_14 = &stack0xfffffffc;
    if ((*(char *)(param_1 + 500) == '\x02') &&
       (puStack_14 = &stack0xfffffffc, *(int *)(param_1 + 0x1f0) == 0)) {
      puStack_14 = &stack0xfffffffc;
      cVar3 = AoWE_TProductionControl_RetryProductionExecution_23EDC2EF
                        (*(undefined4 *)(*(int *)(param_1 + 0x1e8) + 0x44),&local_8);
      if (cVar3 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        FUN_0043fd4c(param_1,0);
      }
    }
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00440f27;
  puStack_18 = (undefined1 *)0x440f1f;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

