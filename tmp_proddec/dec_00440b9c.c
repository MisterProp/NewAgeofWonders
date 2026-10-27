
void MainBtn1Click(int param_1)

{
  uint uVar1;
  int *piVar2;
  undefined1 *puVar3;
  char cVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00440d9c;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  cVar4 = *(char *)(param_1 + 500);
  if (cVar4 == '\0') {
    FUN_0043fd4c(param_1,1);
    FUN_0043f50c(param_1);
  }
  else if (cVar4 == '\x01') {
    uVar1 = *(uint *)(param_1 + 0x1ec);
    if (uVar1 < 4) {
      puStack_14 = &stack0xfffffffc;
      iVar5 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),uVar1);
      if (*(char *)(iVar5 + 0xc) == '\0') {
        cVar4 = PrdPlace_TProductionPlace_CanResearch_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),0,
                           &local_8);
        if (cVar4 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          PrdPlace_TProductionPlace_Research_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),0);
          FUN_0043fd4c(param_1,0);
        }
      }
      else {
        cVar4 = PrdPlace_TProductionPlace_CanProduce_23EDC2EF
                          (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),0,
                           &local_8);
        if (cVar4 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          PrdPlace_TProductionPlace_Produce_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1e8),*(undefined4 *)(param_1 + 0x1ec),0);
          FUN_0043fd4c(param_1,0);
        }
      }
    }
    else if (uVar1 == 4) {
      puStack_14 = &stack0xfffffffc;
      cVar4 = PrdPlace_TProductionPlace_CanTrade_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x1e8),0,&local_8);
      if (cVar4 == '\0') {
        (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                  (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
      }
      else {
        PrdPlace_TProductionPlace_Trade_23EDC2EF(*(undefined4 *)(param_1 + 0x1e8),0);
        FUN_0043fd4c(param_1,0);
      }
    }
    else {
      puStack_14 = &stack0xfffffffc;
      if (uVar1 == 5) {
        puStack_14 = &stack0xfffffffc;
        cVar4 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x1ec))
                          (*(int **)(param_1 + 0x1e8),
                           *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),&local_8);
        if (cVar4 == '\0') {
          (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0x114))
                    (*(int **)AoWE_AoWHSMap_6B957A58_exref,local_8);
        }
        else {
          (**(code **)(**(int **)(param_1 + 0x1e8) + 0x1e8))
                    (*(int **)(param_1 + 0x1e8),
                     *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
          (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
        }
      }
    }
  }
  else {
    puStack_14 = &stack0xfffffffc;
    if ((cVar4 == '\x02') &&
       (iVar5 = *(int *)(*(int *)(param_1 + 0x100) + 0x120), puStack_14 = &stack0xfffffffc,
       iVar5 != -1)) {
      piVar2 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
      puStack_14 = &stack0xfffffffc;
      (**(code **)(*piVar2 + 0xb4))(piVar2,iVar5);
    }
  }
  puVar3 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00440da3;
  puStack_18 = (undefined1 *)0x440d9b;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar3);
  return;
}

