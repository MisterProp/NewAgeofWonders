
void AoWE_TAoWHSMap_GenerateNewTurnOrder_23EDC2EF(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  
                    /* 0x782fc  1397  AoWE.TAoWHSMap.GenerateNewTurnOrder@23EDC2EF */
  iVar1 = System_TObject_Create_23EDC2EF(Engine__TByteList_F5602782_exref,1);
  iVar2 = (**(code **)(**(int **)(param_1 + 0x140) + 0x54))();
  iVar2 = iVar2 + -1;
  if (0 < iVar2) {
    iVar5 = 1;
    do {
      Engine_TByteList_Add_23EDC2EF(iVar1,iVar5);
      iVar5 = iVar5 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x170));
  Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x170),0);
  while (*(int *)(iVar1 + 8) != 0) {
    uVar3 = AoWE_TAoWHSMap_Random_23EDC2EF(param_1,*(int *)(iVar1 + 8));
    uVar4 = Engine_TByteList_Get_23EDC2EF(iVar1,uVar3);
    Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x170),uVar4);
    Engine_TByteList_Delete_23EDC2EF(iVar1,uVar3);
  }
  System_TObject_Free_23EDC2EF(iVar1);
  return;
}

