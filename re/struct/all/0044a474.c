
void FUN_0044a474(int param_1,int param_2)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  
  if (*(int *)(param_2 + 0x120) != -1) {
    piVar1 = *(int **)(*(int *)(param_1 + 200) + 0x114);
    uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,*(int *)(param_2 + 0x120));
    cVar2 = System__IsClass_51F89FF7(uVar3,PrdPlace__TProductionPlace_9A13A4CB_exref);
    if (cVar2 == '\0') {
      piVar1 = *(int **)(*(int *)(param_1 + 200) + 0x114);
      uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(param_2 + 0x120));
      AoWE_TAoWHSMap_Select_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,uVar3,1);
    }
    else {
      piVar1 = *(int **)(*(int *)(param_1 + 200) + 0x114);
      uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(param_2 + 0x120));
      FUN_0043f1f4(*(undefined4 *)PTR_DAT_0045a4c4,uVar3);
    }
  }
  return;
}

