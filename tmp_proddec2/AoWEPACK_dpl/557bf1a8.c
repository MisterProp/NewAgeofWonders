// /AoWEPACK.dpl : 557bf1a8

void PrdPlace_TProductionPlace_GenerateDefenders_23EDC2EF(int *param_1)

{
  char cVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 uVar8;
  int iVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  
                    /* 0xbf1a8  7444  PrdPlace.TProductionPlace.GenerateDefenders@23EDC2EF */
  if (*(char *)((int)param_1 + 0x49) != '\0') {
    if (*(char *)((int)param_1 + 0x49) == '\x04') {
      cVar1 = AoWE_TAoWHSMap_Random_23EDC2EF(*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,3);
      *(char *)((int)param_1 + 0x49) = cVar1 + '\x01';
    }
    iVar9 = (*(byte *)((int)param_1 + 0x49) - 2) * 0x19 + 0x19;
    iVar4 = (**(code **)(*(int *)param_1[1] + 0x80))((int *)param_1[1],0x20217);
    if (iVar4 != 0) {
      iVar4 = AoWE_TArmy_ObtainValue_23EDC2EF(*(undefined4 *)(iVar4 + 0x1c),0xff);
      iVar9 = iVar9 - iVar4;
    }
    if (0 < iVar9) {
      uVar5 = AoWE_TAoWHSMap_Random_23EDC2EF
                        (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,0xfffff);
      *(undefined4 *)System_RandSeed_A0665B6E_exref = uVar5;
      iVar4 = AoWE_TUnitIndexListCollection_GetRndCollection_23EDC2EF
                        (*(undefined4 *)(param_1[2] + 0x5c));
      if (iVar4 != -1) {
        uVar6 = AoWE_TUnitIndexList_Create_23EDC2EF(&AoWE__TUnitIndexList_A715150B,1);
        uVar5 = *(undefined4 *)(param_1[2] + 0x5c);
        iVar7 = AoWE_TUnitIndexListCollection_GetCollectionCount_23EDC2EF(uVar5,iVar4);
        iVar7 = iVar7 + -1;
        if (-1 < iVar7) {
          do {
            uVar8 = AoWE_TUnitIndexListCollection_GetU_23EDC2EF(uVar5,iVar4,iVar7);
            Engine_TIntegerList_Add_23EDC2EF(uVar6,uVar8);
            iVar7 = iVar7 + -1;
          } while (iVar7 != -1);
        }
        uVar11 = 0;
        uVar10 = 4;
        uVar8 = 3;
        uVar5 = uVar6;
        cVar1 = (**(code **)(*param_1 + 0x7c))();
        cVar2 = (**(code **)(*param_1 + 0x78))();
        cVar3 = (**(code **)(*param_1 + 0x74))();
        AoWE_PlaceRandomUnits_6E773D98
                  ((int)cVar3,(int)cVar2,(int)cVar1,uVar8,iVar9,uVar10,uVar5,uVar11);
        iVar4 = AoWE_TAoWMapField_GetArmyHS_23EDC2EF(param_1[1]);
        if ((iVar4 != 0) && (*(char *)(*(int *)(iVar4 + 0x1c) + 0x12) == '\0')) {
          AoWE_TArmy_SetBehavior_23EDC2EF(*(undefined4 *)(iVar4 + 0x1c),0xff,2);
        }
        System_TObject_Free_23EDC2EF(uVar6);
      }
    }
  }
  return;
}

