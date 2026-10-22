
undefined1 ILTer_TILTerrainMO_ValidTerrainType_23EDC2EF(int *param_1,int param_2)

{
  undefined1 uVar1;
  undefined4 uVar2;
  int iVar3;
  
                    /* 0x1873c  958  ILTer.TILTerrainMO.ValidTerrainType@23EDC2EF */
  if (param_2 < 0) {
    uVar1 = 0;
  }
  else {
    uVar2 = (**(code **)(*param_1 + 300))();
    iVar3 = *(int *)(param_1[2] + 0x24);
    if (iVar3 == 0) {
      uVar1 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF
                        (*(undefined4 *)(param_1[2] + 0x20),uVar2);
    }
    else {
      iVar3 = ImageLib_TImageSequenceList_GetImageSequence_23EDC2EF(iVar3,uVar2);
      uVar1 = iVar3 != 0;
    }
  }
  return uVar1;
}

