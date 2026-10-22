
void FUN_0044a5cc(int param_1,int param_2)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  if (*(int *)(param_2 + 0x120) != -1) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x110) + 0x114);
    (**(code **)(*piVar2 + 0x18))(piVar2,*(int *)(param_2 + 0x120));
    piVar2 = (int *)AoWE_TArmy_GetArmyHS_23EDC2EF();
    cVar1 = (**(code **)(*piVar2 + 0x7c))();
    iVar3 = (int)cVar1;
    cVar1 = (**(code **)(*piVar2 + 0x78))();
    iVar4 = (int)cVar1;
    cVar1 = (**(code **)(*piVar2 + 0x74))();
    (**(code **)(**(int **)AoWE_AoWHSMap_6B957A58_exref + 0xbc))
              (*(int **)AoWE_AoWHSMap_6B957A58_exref,(int)cVar1,iVar4,iVar3);
    uVar5 = (**(code **)(*piVar2 + 0x7c))();
    uVar6 = (**(code **)(*piVar2 + 0x78))();
    uVar7 = (**(code **)(*piVar2 + 0x74))();
    AoWE_TAbstractAoWHSMap_PlayCenterAnimation_23EDC2EF
              (*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,uVar7,uVar6,uVar5);
  }
  return;
}

