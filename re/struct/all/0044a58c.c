
void FUN_0044a58c(int param_1,int param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_2 + 0x120) != -1) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x110) + 0x114);
    (**(code **)(*piVar1 + 0x18))(piVar1,*(undefined4 *)(param_2 + 0x120));
    uVar2 = AoWE_TArmy_GetArmyHS_23EDC2EF();
    AoWE_TAoWHSMap_Select_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,uVar2,1);
  }
  return;
}

