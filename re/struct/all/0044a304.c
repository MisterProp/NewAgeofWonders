
void FUN_0044a304(int param_1,int param_2,char param_3)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  if ((param_3 == '\x01') && (*(int *)(param_2 + 0x120) != -1)) {
    piVar2 = *(int **)(*(int *)(param_1 + 0x74) + 0x114);
    piVar2 = (int *)(**(code **)(*piVar2 + 0x18))(piVar2,*(undefined4 *)(param_2 + 0x120));
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

