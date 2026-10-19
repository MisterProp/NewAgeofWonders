// aowepack : AoWE.TAoWHSSet.Create.c
// Ghidra 12.1.2 decompilation of aowepack.dpl (image base per module), function @ 0x5574b9ec.
// Reference for src/engine/pfs.cpp - see docs/01_FORMATS.md (PFS section).

void AoWE_TAoWHSSet_Create_23EDC2EF(int param_1,char param_2)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined1 extraout_DL;
  undefined1 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  
                    /* 0x4b9ec  2880  AoWE.TAoWHSSet.Create@23EDC2EF */
  uStack_8 = 0;
  local_c = 0;
  local_10 = 0;
  uVar4 = 0;
  if (param_2 != '\0') {
    puStack_2c = (undefined1 *)0x5574ba03;
    param_1 = System__ClassCreate_51F89FF7();
    uVar4 = extraout_DL;
  }
  uStack_8 = CONCAT13(uVar4,(undefined3)uStack_8);
  puStack_30 = &LAB_5574c0ee;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  puStack_2c = &stack0xfffffffc;
  HSEngine_THSSet_Create_23EDC2EF(param_1,0);
  uVar2 = Engine_TECustomOwner_Create_23EDC2EF
                    (HSEngine__TMapObjectResourceList_290B5577_exref,1,param_1);
  *(undefined4 *)(param_1 + 0x88) = uVar2;
  Engine_TECustomOwner_SetControlStyle_23EDC2EF(uVar2,3);
  iVar3 = HSEngine_TMORTerrainControl_Create_23EDC2EF
                    (HSEngine__THexagonControl_B96CC3A3_exref,1,param_1);
  *(int *)(param_1 + 0x8c) = iVar3;
  *(code **)(iVar3 + 0x14) = HSEngine__TAbstractHexagon_AD3419BC_exref;
  iVar3 = HSEngine_TMORTerrainControl_Create_23EDC2EF
                    (HSEngine__TRoadControl_47F7FA9F_exref,1,param_1);
  *(int *)(param_1 + 0x90) = iVar3;
  *(code **)(iVar3 + 0x14) = HSEngine__TAbstractRoad_1FDAFEB5_exref;
  System__LStrCat3_51F89FF7(&local_c,*(undefined4 *)(AoWE_AoWEngine_7E705837 + 0x2c),"IMAGES");
  ImageLib_TImageLibraryManager_ExecutePathSearch_23EDC2EF(*(undefined4 *)(param_1 + 0x2c),local_c);
  System__LStrCat3_51F89FF7(&local_c,*(undefined4 *)(AoWE_AoWEngine_7E705837 + 0x2c),&DAT_5574c12c);
  Sound_TSFXManager_ExecutePathSearch_23EDC2EF(*(undefined4 *)(param_1 + 0x30),local_c);
  uVar2 = ImageLib_TImageSequenceList_Create_23EDC2EF
                    (ImageLib__TImageSequenceList_C32858A4_exref,1,*(undefined4 *)(param_1 + 0x2c),0
                    );
  *(undefined4 *)(param_1 + 0x94) = uVar2;
  System_LoadResString_02614E1B(&AoWE_WaterRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_GrassRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_DesertRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_SnowRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_SteppeRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_WastelandRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_IceRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_EarthRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_RockRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_LavaRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_WaterRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_ReservedRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_DirtRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_IceRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_CoastRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_BorderRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x3c) + 0x34))(*(int **)(param_1 + 0x3c),local_c);
  System_LoadResString_02614E1B(&AoWE_MountainRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_DenseVegetationRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_HillRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_VegetationRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_RoadRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_BridgeRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_RubbleRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_StructureRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_ObstacleRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_LightVegetationRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_SolidRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x40) + 0x34))(*(int **)(param_1 + 0x40),local_c);
  System_LoadResString_02614E1B(&AoWE_NoneRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x34))(*(int **)(param_1 + 0x44),local_c);
  System_LoadResString_02614E1B(&AoWE_RocksRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x34))(*(int **)(param_1 + 0x44),local_c);
  System_LoadResString_02614E1B(&AoWE_BonesRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x34))(*(int **)(param_1 + 0x44),local_c);
  System_LoadResString_02614E1B(&AoWE_DeadTreeRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x34))(*(int **)(param_1 + 0x44),local_c);
  System_LoadResString_02614E1B(&AoWE_ToadstoolsRStr_8276FCC3,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x34))(*(int **)(param_1 + 0x44),local_c);
  iVar3 = AoWE_TPadControl_Create_23EDC2EF(&AoWE__TPadControl_211F32E2,1,param_1);
  *(int *)(param_1 + 0x58) = iVar3;
  uVar2 = *(undefined4 *)(iVar3 + 0x10);
  Engine_TBitList_Put_23EDC2EF(uVar2,1,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,2,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,3,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,4,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,5,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,6,1);
  Engine_TBitList_Put_23EDC2EF(uVar2,0xc,1);
  uVar2 = AoWE_TRaceResourceList_Create_23EDC2EF(&AoWE__TRaceResourceList_C71A264A,1,param_1);
  *(undefined4 *)(param_1 + 0x54) = uVar2;
  uVar2 = AoWE_TFlagControl_Create_23EDC2EF(&AoWE__TFlagControl_499FFEAD,1,param_1);
  *(undefined4 *)(param_1 + 0x78) = uVar2;
  uVar2 = AoWE_TFireControl_Create_23EDC2EF(&AoWE__TFireControl_EEB872CD,1);
  *(undefined4 *)(param_1 + 0x7c) = uVar2;
  uVar2 = AoWE_TUnitResourceList_Create_23EDC2EF(&AoWE__TUnitResourceList_4EA0AF20,1,param_1);
  *(undefined4 *)(param_1 + 0x5c) = uVar2;
  uVar2 = AoWE_TUnitGFXResourceList_Create_23EDC2EF(&AoWE__TUnitGFXResourceList_CA037E56,1,param_1);
  *(undefined4 *)(param_1 + 0x60) = uVar2;
  uVar2 = AoWE_THeroResourceList_Create_23EDC2EF(&AoWE__THeroResourceList_66DB465E,1,param_1);
  *(undefined4 *)(param_1 + 100) = uVar2;
  uVar2 = AoWE_TItemGFXList_Create_23EDC2EF(&AoWE__TItemGFXList_0A1BD6C4,1,param_1);
  *(undefined4 *)(param_1 + 0x68) = uVar2;
  uVar2 = AoWE_TFaceResourceList_Create_23EDC2EF(&AoWE__TFaceResourceList_7FE6954D,1,param_1);
  *(undefined4 *)(param_1 + 0x74) = uVar2;
  uVar2 = AoWE_TItemLibraryManager_Create_23EDC2EF(&AoWE__TItemLibraryManager_DF5179A8,1,0);
  *(undefined4 *)(param_1 + 0x6c) = uVar2;
  uVar2 = AoWE_THeroLibraryManager_Create_23EDC2EF(&AoWE__THeroLibraryManager_AA77D8E8,1,0);
  *(undefined4 *)(param_1 + 0x70) = uVar2;
  uVar2 = AoWE_TAbilityControl_Create_23EDC2EF(&AoWE__TAbilityControl_607B97B8,1,0);
  *(undefined4 *)(param_1 + 0x80) = uVar2;
  uVar2 = AoWE_TSpellControl_Create_23EDC2EF(&AoWE__TSpellControl_20E1449F,1,0);
  *(undefined4 *)(param_1 + 0x84) = uVar2;
  uVar2 = Engine_TENode_Create_23EDC2EF(&AoWE__TTextControl_D6A5CDBC,1,0);
  *(undefined4 *)(param_1 + 0x98) = uVar2;
  uVar2 = ImageLib_TImageLibraryManager_LinkToIL_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x2c),1,"GENERAL.ILB");
  *(undefined4 *)(param_1 + 0x48) = uVar2;
  uVar2 = ImageLib_TImageLibraryManager_LinkToIL_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x2c),1,"CURSORS.ILB");
  *(undefined4 *)(param_1 + 0x4c) = uVar2;
  uVar2 = ImageLib_TImageLibraryManager_LinkToIL_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x2c),1,"COMBAT.ILB");
  puVar1 = puStack_2c;
  *(undefined4 *)(param_1 + 0x50) = uVar2;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_5574c0f5;
  puStack_30 = (undefined1 *)0x5574c0ed;
  System__LStrArrayClr_51F89FF7(&local_10,2,puVar1);
  return;
}

