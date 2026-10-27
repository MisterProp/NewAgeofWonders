// /AoWEPACK.dpl : AoWE.TAbstractUnit.ShowEx@23EDC2EF

int AoWE_TAbstractUnit_ShowEx_23EDC2EF
              (int *param_1,int param_2,int param_3,byte *param_4,undefined4 param_5)

{
  char cVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  int local_10;
  
                    /* 0x812ec  980  AoWE.TAbstractUnit.ShowEx@23EDC2EF */
  local_10 = 0;
  uVar2 = (**(code **)(*param_1 + 0x118))();
  piVar3 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                            (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x60),uVar2);
  (**(code **)(*piVar3 + 0x28))();
  if (param_4[2] == 4) {
    local_10 = -1;
  }
  else {
    iVar4 = AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x60),uVar2);
    iVar4 = *(int *)(*(int *)(*(int *)(iVar4 + 0x28) + 4) + -4 +
                    ((uint)param_4[2] * 6 + (uint)*param_4) * 4);
    if (iVar4 == 0) {
      local_10 = -1;
    }
    else {
      if (param_4[8] == 0) {
        iVar5 = ImageLib_TImageSequence_GetTotalSequenceFrames_23EDC2EF(iVar4);
        iVar6 = ImageLib_TImageSequence_GetUnloopedFrameIndex_23EDC2EF
                          (iVar4,*(undefined4 *)(param_4 + 4));
        if (iVar6 < 0) {
          iVar6 = param_3 + *(int *)(iVar4 + 0x28) + 0x14;
          uVar7 = param_5;
          uVar8 = AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                            (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x60),uVar2);
          AoWE_TUnitGFXResource_Show_23EDC2EF
                    (uVar8,*param_4,param_2 + *(int *)(iVar4 + 0x24) + 0x10,uVar7,iVar6);
        }
        else {
          uVar7 = param_5;
          piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(iVar4 + 0x10),iVar6);
          (**(code **)(*piVar3 + 0x84))
                    (piVar3,param_2 + *(int *)(iVar4 + 0x24) + 0x10,
                     param_3 + *(int *)(iVar4 + 0x28) + 0x14,uVar7);
        }
        if (0 < iVar5 - *(int *)(param_4 + 4)) {
          local_10 = iVar5 - *(int *)(param_4 + 4);
        }
      }
      else {
        iVar5 = ImageLib_TImageSequence_GetFrameIndex_23EDC2EF
                          (iVar4,*(undefined4 *)(param_4 + 4),(uint)param_1 >> 2 & 0xffff);
        if (-1 < iVar5) {
          uVar7 = param_5;
          piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(iVar4 + 0x10),iVar5);
          (**(code **)(*piVar3 + 0x84))
                    (piVar3,param_2 + *(int *)(iVar4 + 0x24) + 0x10,
                     param_3 + *(int *)(iVar4 + 0x28) + 0x14,uVar7);
        }
        local_10 = 1;
      }
      for (iVar4 = *(int *)(iVar4 + 0xc); iVar4 != 0; iVar4 = *(int *)(iVar4 + 0xc)) {
        iVar5 = ImageLib_TImageSequence_GetFrameIndex_23EDC2EF(iVar4,*(undefined4 *)(param_4 + 4),0)
        ;
        if (-1 < iVar5) {
          uVar7 = param_5;
          piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(iVar4 + 0x10),iVar5);
          (**(code **)(*piVar3 + 0x84))
                    (piVar3,param_2 + *(int *)(iVar4 + 0x24) + 0x10,
                     param_3 + *(int *)(iVar4 + 0x28) + 0x14,uVar7);
        }
      }
    }
  }
  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x60);
  if (cVar1 != '\0') {
    if (param_4[8] == 0) {
      iVar4 = param_3 + 0x20;
      uVar8 = 4;
      uVar7 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x60);
      ImageLib_TImageSequenceList_Show_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar7,uVar8,iVar4);
    }
    else {
      iVar4 = param_3 + 0x20;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x60);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
  }
  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x62);
  if (cVar1 != '\0') {
    if (param_4[8] == 0) {
      iVar4 = param_3 + 0x20;
      uVar8 = 4;
      uVar7 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x62);
      ImageLib_TImageSequenceList_Show_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar7,uVar8,iVar4);
    }
    else {
      iVar4 = param_3 + 0x20;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x62);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
  }
  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x5d);
  if (cVar1 != '\0') {
    if (param_4[8] == 0) {
      iVar4 = param_3 + 0x20;
      uVar8 = 4;
      uVar7 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x5d);
      ImageLib_TImageSequenceList_Show_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar7,uVar8,iVar4);
    }
    else {
      iVar4 = param_3 + 0x20;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x5d);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
  }
  if ((param_4[1] & 1) != 0) {
    uVar7 = (**(code **)(*param_1 + 0xd0))();
    uVar8 = (**(code **)(*param_1 + 0xe0))();
    uVar7 = AoWE_HitPointsToHitPcnt_39FFD8AD(uVar8,uVar7);
    AoWE_ShowHitPcnt_7E58D268(param_2,param_3,param_5,uVar7);
  }
  if ((param_4[1] & 4) != 0) {
    cVar1 = (**(code **)(*param_1 + 0xe0))();
    if (cVar1 == '\0') {
      uVar7 = param_5;
      piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x50),4);
      (**(code **)(*piVar3 + 0x84))(piVar3,param_2 + 0x12,param_3 + 10,uVar7);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x30);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x30);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x96);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x96);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x95);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x95);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x61);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x61);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x5c);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x23;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x5c);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x5f);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x20;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x5f);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x5e);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x23;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x5e);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x7f);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x23;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x7f);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x6b);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x6b);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
    cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x22);
    if (cVar1 != '\0') {
      iVar4 = param_3 + 0x28;
      uVar7 = *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40);
      uVar8 = param_5;
      iVar5 = AoWE_TAbilityControl_GetAbility_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),0x22);
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(iVar5 + 0x1c),0,param_2 + 0x20,uVar8,uVar7,iVar4);
    }
  }
  if ((*(byte *)((int)param_1 + 0x25) & 0x20) != 0) {
    if (param_4[8] == 0) {
      ImageLib_TImageSequenceList_Show_23EDC2EF
                (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x94),0x46,param_2 + 0x20,param_5,5,
                 param_3 + 0x28);
    }
    else {
      ImageLib_TImageSequenceList_ShowLooped_23EDC2EF
                (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x94),0x46,param_2 + 0x20,param_5,
                 *(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x40),param_3 + 0x28);
    }
  }
  piVar3 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                            (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x60),uVar2);
  (**(code **)(*piVar3 + 0x2c))();
  return local_10;
}

