
void FUN_0043af24(int param_1,undefined4 param_2)

{
  uint uVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int *piVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iStack_20;
  int iStack_1c;
  int iStack_14;
  
  FUN_0043ad14(param_1);
  *(undefined4 *)(param_1 + 0x2b0) = param_2;
  FUN_0043b208(param_1);
  FUN_0043bb14(param_1,0);
  FUN_0043ac6c(param_1);
  iStack_14 = (**(code **)(**(int **)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x1c) + 0x54))();
  if (-1 < iStack_14 + -1) {
    iVar10 = 0;
    do {
      iVar5 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x1c),iVar10);
      FUN_0043acd4(param_1,*(undefined4 *)(iVar5 + 8));
      iVar10 = iVar10 + 1;
      iStack_14 = iStack_14 + -1;
    } while (iStack_14 != 0);
  }
  uVar6 = AoWTC_TRndTacHsm_Create_23EDC2EF(AoWTC__TRndTacHsm_exref,1);
  cVar2 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x78))();
  iVar10 = (int)cVar2;
  cVar2 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x7c))();
  iVar5 = (int)cVar2;
  cVar2 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x74))();
  uVar7 = AoWTC_TRndTacHsm_RandomMap_23EDC2EF(uVar6,0,(int)cVar2,iVar5,iVar10);
  *(undefined4 *)(param_1 + 0x2b4) = uVar7;
  System_TObject_Free_23EDC2EF(uVar6);
  piVar8 = (int *)Engine_TEChangeNotifyEventList_Create_23EDC2EF
                            (AoWE__TArmyHSList_781A5FA7_exref,1,0);
  (**(code **)(**(int **)(param_1 + 0x2b0) + 0x1d8))(*(int **)(param_1 + 0x2b0),piVar8);
  iStack_1c = (**(code **)(*piVar8 + 0x54))();
  if (-1 < iStack_1c + -1) {
    iStack_20 = 0;
    do {
      piVar9 = (int *)AoWE_TArmyHSList_GetArmyHS_23EDC2EF(piVar8,iStack_20);
      cVar2 = (**(code **)(*piVar9 + 0x78))();
      iVar10 = (int)cVar2;
      cVar2 = (**(code **)(*piVar9 + 0x74))();
      cVar3 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x78))();
      cVar4 = (**(code **)(**(int **)(param_1 + 0x2b0) + 0x74))();
      HSEngine_dHXtoHN_F769B863((int)cVar4,(int)cVar3,(int)cVar2,iVar10);
      iVar10 = (**(code **)(*(int *)piVar9[7] + 0x54))();
      if (-1 < iVar10 + -1) {
        iVar11 = 0;
        do {
          iVar12 = iVar11;
          uVar6 = AoWE_TUnitList_GetUnit_23EDC2EF(piVar9[7],iVar11);
          OverviewUnit_PlaceOverviewUnit_2B1F96C2
                    (*(undefined4 *)(param_1 + 0x2b4),uVar6,iVar5,iVar12);
          iVar11 = iVar11 + 1;
          iVar10 = iVar10 + -1;
        } while (iVar10 != 0);
      }
      iStack_20 = iStack_20 + 1;
      iStack_1c = iStack_1c + -1;
    } while (iStack_1c != 0);
  }
  System_TObject_Free_23EDC2EF(piVar8);
  piVar8 = *(int **)(*(int *)(param_1 + 0x2b4) + 0x10c);
  if (piVar8 == (int *)0x0) {
    *(undefined4 *)(param_1 + 0x2b8) = 800;
    *(undefined4 *)(param_1 + 700) = 800;
  }
  else {
    iVar10 = param_1 + 700;
    cVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x2b4) + 0x10c) + 0x78))();
    cVar3 = (**(code **)(*piVar8 + 0x74))();
    HSEngine_HXtoHP_8E9B8701((int)cVar3,(int)cVar2,param_1 + 0x2b8,iVar10);
    uVar1 = *(uint *)(*(int *)(param_1 + 0x29c) + 0x7c);
    iVar10 = (int)uVar1 >> 1;
    if (iVar10 < 0) {
      iVar10 = iVar10 + (uint)((uVar1 & 1) != 0);
    }
    *(int *)(param_1 + 0x2b8) = (*(int *)(param_1 + 0x2b8) + 0x10) - iVar10;
    uVar1 = *(uint *)(*(int *)(param_1 + 0x29c) + 0x80);
    iVar10 = (int)uVar1 >> 1;
    if (iVar10 < 0) {
      iVar10 = iVar10 + (uint)((uVar1 & 1) != 0);
    }
    *(int *)(param_1 + 700) = (*(int *)(param_1 + 700) + 0x10) - iVar10;
  }
  AoWE_TTutorialControl_ShowMessage_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x38,1,1);
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  return;
}

