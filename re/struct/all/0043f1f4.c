
void FUN_0043f1f4(int param_1,int *param_2)

{
  uint uVar1;
  undefined1 *puVar2;
  char cVar3;
  char cVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  int *in_FS_OFFSET;
  int iVar11;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  int local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_0043f427;
  puStack_24 = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = (int)&puStack_24;
  FUN_0043efa0(param_1);
  *(int **)(param_1 + 0x1e8) = param_2;
  FUN_0043f574(param_1);
  FUN_0043fd4c(param_1,0);
  piVar5 = (int *)(**(code **)(*param_2 + 0x144))();
  (**(code **)(*piVar5 + 0x1c))(piVar5,&local_c);
  piVar5 = *(int **)(*(int *)(param_1 + 0x130) + 0x118);
  (**(code **)(*piVar5 + 0x2c))(piVar5,local_c);
  iVar6 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c) + 0x54))();
  if (-1 < iVar6 + -1) {
    iVar10 = 0;
    do {
      iVar7 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),iVar10);
      FUN_0043ef60(param_1,*(undefined4 *)(iVar7 + 8));
      iVar10 = iVar10 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
  FUN_0043f1d8(param_1);
  uVar8 = AoWTC_TRndTacHsm_Create_23EDC2EF(AoWTC__TRndTacHsm_exref,1);
  cVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x78))();
  iVar6 = (int)cVar3;
  cVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x7c))();
  iVar10 = (int)cVar3;
  cVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x74))();
  uVar9 = AoWTC_TRndTacHsm_RandomMap_23EDC2EF(uVar8,0,(int)cVar3,iVar10,iVar6);
  *(undefined4 *)(param_1 + 0x1f8) = uVar9;
  System_TObject_Free_23EDC2EF(uVar8);
  local_8 = AoWE_TAoWMapField_GetArmyHS_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 0x1e8) + 4));
  if (local_8 != 0) {
    iVar6 = (**(code **)(**(int **)(local_8 + 0x1c) + 0x54))();
    if (-1 < iVar6 + -1) {
      iVar7 = 0;
      do {
        iVar11 = iVar7;
        uVar8 = AoWE_TUnitList_GetUnit_23EDC2EF(*(undefined4 *)(local_8 + 0x1c),iVar7);
        OverviewUnit_PlaceOverviewUnit_2B1F96C2(*(undefined4 *)(param_1 + 0x1f8),uVar8,0,iVar11);
        iVar7 = iVar7 + 1;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
    }
  }
  iVar6 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x22c))();
  *(bool *)(*(int *)(param_1 + 0xb8) + 0xc0) = iVar6 != 0;
  piVar5 = *(int **)(*(int *)(param_1 + 0x1f8) + 0x10c);
  if (piVar5 == (int *)0x0) {
    *(undefined4 *)(param_1 + 0x1e0) = 800;
    *(undefined4 *)(param_1 + 0x1e4) = 800;
  }
  else {
    iVar6 = param_1 + 0x1e4;
    cVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x1f8) + 0x10c) + 0x78))();
    cVar4 = (**(code **)(*piVar5 + 0x74))();
    HSEngine_HXtoHP_8E9B8701((int)cVar4,(int)cVar3,param_1 + 0x1e0,iVar6);
    uVar1 = *(uint *)(*(int *)(param_1 + 0x1d0) + 0x7c);
    iVar6 = (int)uVar1 >> 1;
    if (iVar6 < 0) {
      iVar6 = iVar6 + (uint)((uVar1 & 1) != 0);
    }
    *(int *)(param_1 + 0x1e0) = (*(int *)(param_1 + 0x1e0) + 0x10) - iVar6;
    uVar1 = *(uint *)(*(int *)(param_1 + 0x1d0) + 0x80);
    iVar6 = (int)uVar1 >> 1;
    if (iVar6 < 0) {
      iVar6 = iVar6 + (uint)((uVar1 & 1) != 0);
    }
    *(int *)(param_1 + 0x1e4) = (*(int *)(param_1 + 0x1e4) + 0x10) - iVar6;
  }
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  puVar2 = puStack_24;
  *in_FS_OFFSET = iVar10;
  puStack_24 = &LAB_0043f42e;
  System__LStrClr_40929B27(&local_c,iVar10,puVar2);
  return;
}

