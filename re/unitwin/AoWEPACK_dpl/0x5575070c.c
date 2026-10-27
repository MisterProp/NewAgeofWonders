// /AoWEPACK.dpl : 0x5575070c

void AoWE_TAbilityControl_ListAbilityInfo_23EDC2EF
               (int param_1,int *param_2,char param_3,undefined1 param_4)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  
                    /* 0x5070c  2679  AoWE.TAbilityControl.ListAbilityInfo@23EDC2EF */
  iVar2 = System_TObject_Create_23EDC2EF(Classes__TList_9F2060BF_exref,1);
  iVar4 = *(int *)(*(int *)(param_1 + 8) + 8);
  if (-1 < iVar4 + -1) {
    iVar5 = 0;
    do {
      iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar5 * 4);
      if (iVar1 != 0) {
        Classes_TList_Add_23EDC2EF(iVar2,iVar1);
      }
      iVar5 = iVar5 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  if (param_3 != '\0') {
    Classes_TList_Sort_23EDC2EF(iVar2,&LAB_557506a0);
  }
  iVar4 = *(int *)(iVar2 + 8);
  if (-1 < iVar4 + -1) {
    iVar5 = 0;
    do {
      piVar3 = (int *)Classes_TList_Get_23EDC2EF(iVar2,iVar5);
      (**(code **)(*piVar3 + 0x100))(piVar3,param_2,param_4);
      (**(code **)(*param_2 + 0x34))(param_2,"-------------------------------------------------");
      (**(code **)(*param_2 + 0x34))(param_2,0);
      iVar5 = iVar5 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  System_TObject_Free_23EDC2EF(iVar2);
  return;
}

