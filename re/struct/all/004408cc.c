
void FUN_004408cc(int param_1,int param_2,undefined4 *param_3)

{
  uint uVar1;
  int *piVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined1 *puStack_74;
  int *piStack_70;
  undefined1 *local_6c;
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
  
  puVar6 = local_68;
  for (iVar5 = 0xf; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar6 = *param_3;
    param_3 = param_3 + 1;
    puVar6 = puVar6 + 1;
  }
  local_6c = (undefined1 *)
             (*(int *)(*(int *)(param_1 + 0x118) + 0x88) + *(int *)(param_2 + 0x88) +
             *(int *)(param_2 + 0x80));
  piStack_70 = &local_20;
  puStack_74 = (undefined1 *)0x44093d;
  Classes_Rect_9923E97E
            (*(int *)(*(int *)(param_1 + 0x118) + 0x84) + *(int *)(param_2 + 0x84),
             *(int *)(*(int *)(param_1 + 0x118) + 0x88) + *(int *)(param_2 + 0x88),
             *(int *)(*(int *)(param_1 + 0x118) + 0x84) + *(int *)(param_2 + 0x84) +
             *(int *)(param_2 + 0x7c));
  local_6c = (undefined1 *)local_68;
  uVar1 = *(uint *)(param_2 + 0xc);
  if (uVar1 < 4) {
    piVar4 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
    local_6c = (undefined1 *)0x44096b;
    piVar4 = (int *)(**(code **)(*piVar4 + 0x9c))(piVar4,uVar1);
    if (piVar4 == (int *)0x0) {
      piStack_70 = (int *)0x440ab0;
      local_6c = (undefined1 *)local_68;
      piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),0x3e);
      piStack_70 = (int *)0x440ac6;
      (**(code **)(*piVar4 + 0x84))(piVar4,local_20 + 2,iStack_1c + 2);
    }
    else {
      uStack_2c = 3;
      uStack_2b = 0;
      uStack_2a = 0;
      uStack_28 = 0;
      uStack_24 = uStack_24 & 0xffffff00;
      local_6c = (undefined1 *)0x4409a7;
      iVar5 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),
                         *(undefined4 *)(param_2 + 0xc));
      if (*(char *)(iVar5 + 0xc) == '\0') {
        piStack_70 = (int *)0x4409c9;
        local_6c = (undefined1 *)local_68;
        piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),5);
        iVar5 = local_20 + 2;
        piStack_70 = (int *)0x4409e3;
        (**(code **)(*piVar2 + 0x84))(piVar2,iVar5,iStack_1c + 2);
        auStack_30[0] = 5;
        if (piVar4 != (int *)0x0) {
          puStack_74 = auStack_30;
          piStack_70 = (int *)&local_6c;
          (**(code **)(*piVar4 + 0x1a4))(piVar4,uStack_24 - 4,local_20);
        }
        piStack_70 = (int *)&local_6c;
        puStack_74 = (undefined1 *)0x440a19;
        puStack_74 = (undefined1 *)
                     GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                               (*(undefined4 *)GFXE_AlphaBlendTable_AB1D79F5_exref,0x2d);
        piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),5);
        (**(code **)(*piVar2 + 0x94))(piVar2,iVar5,iStack_1c + 2);
      }
      else {
        piStack_70 = (int *)0x440a57;
        local_6c = (undefined1 *)local_68;
        piVar2 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),4);
        piStack_70 = (int *)0x440a71;
        (**(code **)(*piVar2 + 0x84))(piVar2,local_20 + 2,iStack_1c + 2);
        if (piVar4 != (int *)0x0) {
          (**(code **)(*piVar4 + 0x1a4))(piVar4,local_20 + -4,iStack_1c,auStack_38,&puStack_74);
        }
      }
      (**(code **)(*piVar4 + 0x2c))();
    }
  }
  else if (uVar1 == 4) {
    local_6c = (undefined1 *)0x440ad9;
    iVar5 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x22c))();
    if (iVar5 == 0) {
      piStack_70 = (int *)0x440b49;
      local_6c = (undefined1 *)local_68;
      piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),0x3e);
      piStack_70 = (int *)0x440b5f;
      (**(code **)(*piVar4 + 0x84))(piVar4,local_20 + 2,iStack_1c + 2);
    }
    else {
      local_6c = (undefined1 *)0x440aeb;
      iVar5 = Engine_TEObject_Create_23EDC2EF(AoWE__TProductionItem_E3617239_exref,1,0);
      *(undefined4 *)(iVar5 + 8) = 10;
      piStack_70 = (int *)0x440afe;
      local_6c = (undefined1 *)local_68;
      uVar3 = FUN_0043f534(param_1,iVar5);
      piStack_70 = (int *)0x440b12;
      piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),uVar3);
      piStack_70 = (int *)0x440b28;
      (**(code **)(*piVar4 + 0x84))(piVar4,local_20 + 2,iStack_1c + 2);
      piStack_70 = (int *)0x440b2f;
      System_TObject_Free_23EDC2EF(iVar5);
    }
  }
  else if (uVar1 == 5) {
    piStack_70 = (int *)0x440b79;
    piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xec),0x2e);
    piStack_70 = (int *)0x440b8f;
    (**(code **)(*piVar4 + 0x84))(piVar4,local_20 + 2,iStack_1c + 2);
  }
  return;
}

