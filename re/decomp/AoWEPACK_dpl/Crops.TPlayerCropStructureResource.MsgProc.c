
void Crops_TPlayerCropStructureResource_MsgProc_23EDC2EF(int param_1,int *param_2)

{
  undefined4 uVar1;
  
                    /* 0xa6a90  6718  Crops.TPlayerCropStructureResource.MsgProc@23EDC2EF */
  AoWE_TPlayerStructureResource_MsgProc_23EDC2EF(param_1,param_2);
  if (*param_2 == 0x20000008) {
    if (*(int *)(param_1 + 0x74) != 0) {
      System_TObject_Free_23EDC2EF();
    }
    uVar1 = HSEngine_TTerrainList_CreateSurroundingHNTable_23EDC2EF(*(undefined4 *)(param_1 + 0x28))
    ;
    *(undefined4 *)(param_1 + 0x74) = uVar1;
  }
  return;
}

