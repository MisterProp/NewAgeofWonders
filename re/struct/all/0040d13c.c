
void FUN_0040d13c(int param_1,int param_2,undefined4 *param_3)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_70;
  undefined1 *puStack_6c;
  undefined1 *puStack_68;
  undefined4 local_58;
  int local_54;
  int local_50;
  undefined4 local_44 [15];
  int *local_8;
  
  puStack_68 = &stack0xfffffffc;
  local_58 = 0;
  puVar5 = local_44;
  for (iVar4 = 0xf; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *param_3;
    param_3 = param_3 + 1;
    puVar5 = puVar5 + 1;
  }
  puStack_6c = &LAB_0040d336;
  uStack_70 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_70;
  iVar4 = *(int *)(*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) + 0x20);
  local_8 = (int *)AoWE_THeroList_GetHero_23EDC2EF
                             (*(undefined4 *)(iVar4 + 0xc),
                              *(int *)(param_1 + 0x1a8) + *(int *)(param_2 + 0xc));
  if (local_8 != (int *)0x0) {
    Classes_Rect_9923E97E
              (*(int *)(*(int *)(param_1 + 0xbc) + 0x84) + *(int *)(param_2 + 0x84) + 1,
               *(int *)(*(int *)(param_1 + 0xbc) + 0x88) + *(int *)(param_2 + 0x88) + 2,
               *(int *)(*(int *)(param_1 + 0xbc) + 0x84) + *(int *)(param_2 + 0x84) +
               *(int *)(param_2 + 0x7c) + -2,&local_54,
               *(int *)(*(int *)(param_1 + 0xbc) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80) + -2);
    local_54 = local_54 + 1;
    AoWE_TAbstractUnit_ShowFace_23EDC2EF(local_8,&local_54,local_44);
    local_54 = local_54 + -1;
    iVar4 = Engine_TIntegerList_IndexOf_23EDC2EF
                      (*(undefined4 *)(iVar4 + 0x28),
                       *(int *)(param_1 + 0x1a8) + *(int *)(param_2 + 0xc));
    if (iVar4 != -1) {
      puVar5 = local_44;
      uVar2 = GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                        (*(undefined4 *)GFXE_AlphaBlendTable_AB1D79F5_exref,0x41);
      piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),5);
      (**(code **)(*piVar3 + 0x94))(piVar3,local_54,local_50,uVar2,puVar5);
    }
    if (*(char *)(param_2 + 0x134) != '\0') {
      puVar5 = local_44;
      uVar2 = GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                        (*(undefined4 *)GFXE_IntensityBlendTable_AB1D79F5_exref,0x32);
      piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xf0),0x14);
      (**(code **)(*piVar3 + 0x94))(piVar3,local_54 + -1,local_50 + -2,uVar2,puVar5);
    }
    puVar5 = local_44;
    piVar3 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xf0),0x1f);
    iVar4 = local_54;
    (**(code **)(*piVar3 + 0x84))(piVar3,local_54 + 10,local_50 + 0x37,puVar5);
    ImageLib_TImageLibraryFont_SetColor_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x44),0xffffff);
    uVar2 = (**(code **)(*local_8 + 0x124))();
    SysUtils_IntToStr_0F6FDFF6(uVar2,&local_58);
    ImageLib_TImageLibraryFont_SetTextCentered_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x44),iVar4 + 0x18,local_50 + 0x38,
               local_44,local_58);
  }
  puVar1 = puStack_68;
  *in_FS_OFFSET = uStack_70;
  puStack_68 = &LAB_0040d33d;
  puStack_6c = (undefined1 *)0x40d335;
  System__LStrClr_40929B27(&local_58,uStack_70,puVar1);
  return;
}

