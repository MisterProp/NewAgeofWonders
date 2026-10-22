
void FUN_00455d10(int param_1)

{
  char cVar1;
  byte bVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  
  *(undefined4 *)(param_1 + 0x7c) = 0;
  iVar5 = *(int *)(*(int *)(param_1 + 0x78) + 8);
  if (-1 < iVar5 + -1) {
    iVar4 = 0;
    do {
      Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x78),iVar4);
      System_TObject_Free_23EDC2EF();
      iVar4 = iVar4 + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  Classes_TList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x78));
  iVar5 = 0;
  do {
    cVar1 = (**(code **)(**(int **)(param_1 + 0xc0) + 0x148))(*(int **)(param_1 + 0xc0),iVar5);
    if (cVar1 != '\0') {
      iVar4 = System_TObject_Create_23EDC2EF(AoWTC__TTCAbility_EDC5062F_exref,1);
      *(int *)(iVar4 + 4) = iVar5;
      if (*(char *)(*(int *)(DAT_0045b2c0 + 0xc0) + 0x24) ==
          *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
        piVar3 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                  (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                   iVar5);
        bVar2 = (**(code **)(*piVar3 + 0x98))(piVar3,*(undefined4 *)(DAT_0045b2c0 + 0xc0));
        if ((bVar2 & 1) != 0) {
          Classes_TList_Insert_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x78),*(undefined4 *)(param_1 + 0x7c),iVar4);
          *(int *)(param_1 + 0x7c) = *(int *)(param_1 + 0x7c) + 1;
          goto LAB_00455de0;
        }
      }
      Classes_TList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x78),iVar4);
    }
LAB_00455de0:
    iVar5 = iVar5 + 1;
    if (iVar5 == 0xaa) {
      return;
    }
  } while( true );
}

