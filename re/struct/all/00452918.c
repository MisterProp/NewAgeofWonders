
void FUN_00452918(int param_1,int param_2,int param_3,LPRECT param_4)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  RECT local_18;
  int local_8;
  
  param_4->left = 0;
  param_4->right = 0;
  param_4->top = 0;
  param_4->bottom = 0;
  if (param_2 != -1) {
    local_8 = param_3;
    if (*(int *)AoWE_AoWHSMap_6B957A58_exref == 0) {
      if (*(short *)(param_1 + 0x1e8) != 1) {
        cVar1 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x50),
                           (int)*(short *)(param_1 + 0x1e8));
        if (cVar1 != '\0') {
          piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0x50),
                                     (int)*(short *)(param_1 + 0x1e8));
          (**(code **)(*piVar4 + 0x38))(piVar4,&local_18);
          local_18.left = local_18.left + param_2 + -0x19;
          local_18.right = param_2 + local_18.right + -0x19;
          local_18.top = local_18.top + local_8 + -0x19;
          local_18.bottom = local_18.bottom + local_8 + -0x19;
          UnionRect(param_4,param_4,&local_18);
        }
      }
    }
    else {
      if (*(int *)(param_1 + 0x1f8) != 0) {
        (**(code **)(**(int **)(param_1 + 0x1f8) + 0x38))(*(int **)(param_1 + 0x1f8),&local_18);
        local_18.left = local_18.left + param_2 + -0xf;
        local_18.right = local_18.right + param_2 + -0xf;
        local_18.top = local_18.top + local_8 + -0x14;
        local_18.bottom = local_18.bottom + local_8 + -0x14;
        UnionRect(param_4,param_4,&local_18);
      }
      iVar2 = (int)*(short *)(param_1 + 0x1e8);
      if ((iVar2 == 9999) && (*(int *)AoWE_AoWHSMap_6B957A58_exref != 0)) {
        iVar2 = (**(code **)**(undefined4 **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc))();
      }
      if (iVar2 != 1) {
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x4c),iVar2);
        if (iVar3 != 0) {
          piVar4 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x4c),
                                     iVar2);
          (**(code **)(*piVar4 + 0x38))(piVar4,&local_18);
          local_18.left = local_18.left + param_2 + -0x19;
          local_18.right = param_2 + local_18.right + -0x19;
          local_18.top = local_18.top + local_8 + -0x19;
          local_18.bottom = local_18.bottom + local_8 + -0x19;
          UnionRect(param_4,param_4,&local_18);
        }
      }
    }
  }
  return;
}

