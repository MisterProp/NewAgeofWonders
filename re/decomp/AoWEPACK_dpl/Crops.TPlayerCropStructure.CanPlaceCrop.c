
undefined1 Crops_TPlayerCropStructure_CanPlaceCrop_23EDC2EF(int *param_1,int *param_2)

{
  bool bVar1;
  bool bVar2;
  char cVar3;
  byte bVar4;
  undefined1 uVar5;
  int iVar6;
  int *piVar7;
  
                    /* 0xa651c  6728  Crops.TPlayerCropStructure.CanPlaceCrop@23EDC2EF */
  cVar3 = (**(code **)(*param_1 + 0x150))();
  if (cVar3 != '\0') {
    return 0;
  }
  iVar6 = (**(code **)(*param_2 + 0x80))(param_2,0x20217);
  if ((iVar6 == 0) ||
     (cVar3 = AoWE_TPlayerList_GetDiplomaticRelation_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         *(undefined1 *)(*(int *)(iVar6 + 0x1c) + 0x12),(char)param_1[0xc]),
     cVar3 != '\x01')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  bVar2 = false;
  if (bVar1) {
    piVar7 = (int *)HSEngine_TMapField_GetMaxLevelNoneTransparentHS_23EDC2EF(param_2);
    if ((piVar7 == (int *)0x0) || (bVar4 = (**(code **)(*piVar7 + 100))(), bVar4 < 0x1a)) {
      bVar2 = true;
    }
    else {
      bVar2 = false;
    }
  }
  if (!bVar2) {
    return 0;
  }
  bVar4 = (char)param_2[5] - 1;
  if ((bVar4 < 4) || (bVar4 == 0xb)) {
LAB_557a65c2:
    uVar5 = 1;
  }
  else {
    if ((char)param_2[5] == '\x05') {
      cVar3 = (**(code **)(*param_1 + 0x238))();
      if (cVar3 == '\v') goto LAB_557a65c2;
    }
    uVar5 = 0;
  }
  return uVar5;
}

