
void FUN_0043d950(int param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined1 *puStack_74;
  int *piStack_70;
  undefined1 *puStack_6c;
  undefined4 local_68 [12];
  undefined1 auStack_38 [8];
  undefined1 auStack_30 [4];
  undefined1 uStack_2c;
  undefined1 uStack_2b;
  undefined1 uStack_2a;
  undefined4 uStack_28;
  uint uStack_24;
  int local_20;
  int iStack_1c;
  
  puVar4 = local_68;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  puStack_6c = (undefined1 *)
               (*(int *)(*(int *)(param_1 + 0x134) + 0x88) + *(int *)(param_2 + 0x88) +
               *(int *)(param_2 + 0x80));
  piStack_70 = &local_20;
  puStack_74 = (undefined1 *)0x43d9c1;
  Classes_Rect_9923E97E
            (*(int *)(*(int *)(param_1 + 0x134) + 0x84) + *(int *)(param_2 + 0x84),
             *(int *)(*(int *)(param_1 + 0x134) + 0x88) + *(int *)(param_2 + 0x88),
             *(int *)(*(int *)(param_1 + 0x148) + 0x84) + *(int *)(param_2 + 0x84) +
             *(int *)(param_2 + 0x7c));
  puStack_6c = (undefined1 *)0x43d9d5;
  iVar3 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                    (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x2b0) + 0x50) + 0x1c),
                     *(undefined4 *)(param_2 + 0xc));
  if (iVar3 == 0) {
    piStack_70 = (int *)0x43db1d;
    puStack_6c = (undefined1 *)local_68;
    piVar1 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),0x3e);
    piStack_70 = (int *)0x43db33;
    (**(code **)(*piVar1 + 0x84))(piVar1,local_20 + 2,iStack_1c + 2);
  }
  else {
    piVar1 = *(int **)(*(int *)(param_1 + 0x2b0) + 0x50);
    puStack_6c = (undefined1 *)0x43d9f3;
    piVar1 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(param_2 + 0xc));
    uStack_2c = 3;
    uStack_2b = 0;
    uStack_2a = 0;
    uStack_28 = 0;
    uStack_24 = uStack_24 & 0xffffff00;
    if (*(char *)(iVar3 + 0xc) == '\0') {
      piStack_70 = (int *)0x43da35;
      puStack_6c = (undefined1 *)local_68;
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),5);
      iVar3 = local_20 + 2;
      piStack_70 = (int *)0x43da4f;
      (**(code **)(*piVar2 + 0x84))(piVar2,iVar3,iStack_1c + 2);
      auStack_30[0] = 5;
      if (piVar1 != (int *)0x0) {
        puStack_74 = auStack_30;
        piStack_70 = (int *)&puStack_6c;
        (**(code **)(*piVar1 + 0x1a4))(piVar1,uStack_24 - 4,local_20);
      }
      piStack_70 = (int *)&puStack_6c;
      puStack_74 = (undefined1 *)0x43da85;
      puStack_74 = (undefined1 *)
                   GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                             (*(undefined4 *)GFXE_AlphaBlendTable_AB1D79F5_exref,0x2d);
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),5);
      (**(code **)(*piVar2 + 0x94))(piVar2,iVar3,iStack_1c + 2);
    }
    else {
      piStack_70 = (int *)0x43dac3;
      puStack_6c = (undefined1 *)local_68;
      piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),4);
      piStack_70 = (int *)0x43dadd;
      (**(code **)(*piVar2 + 0x84))(piVar2,local_20 + 2,iStack_1c + 2);
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x1a4))(piVar1,local_20 + -4,iStack_1c,auStack_38,&puStack_74);
      }
    }
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x2c))();
    }
  }
  return;
}

