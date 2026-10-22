
void AoWE_TPlayer_UpdateTurnInfo_23EDC2EF(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  undefined2 extraout_var;
  int iStack_18;
  int iStack_14;
  
                    /* 0x52d70  2601  AoWE.TPlayer.UpdateTurnInfo@23EDC2EF */
  iVar2 = Engine_TEObject_Create_23EDC2EF
                    (&AoWE__TTurnInfoItem_FFBF75A9,1,*(undefined4 *)(*(int *)(param_1 + 0x60) + 8));
  if (*(char *)(param_1 + 0x44) != '\x02') {
    *(undefined4 *)(iVar2 + 8) = *(undefined4 *)(param_1 + 0xc4);
    *(undefined4 *)(iVar2 + 0xc) = *(undefined4 *)(*(int *)(param_1 + 0x54) + 0x24);
    uVar3 = (**(code **)(**(int **)(param_1 + 0x1c) + 0x54))();
    *(undefined4 *)(iVar2 + 0x10) = uVar3;
    uVar1 = (**(code **)(**(int **)(param_1 + 0x80) + 0x54))();
    *(undefined1 *)(iVar2 + 0x18) = uVar1;
    iStack_14 = (**(code **)(**(int **)(param_1 + 0x28) + 0x54))();
    if (-1 < iStack_14 + -1) {
      iStack_18 = 0;
      do {
        piVar4 = (int *)AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(param_1 + 0x28),iStack_18);
        iVar5 = (**(code **)(*piVar4 + 0x54))();
        iVar5 = iVar5 + -1;
        if (-1 < iVar5) {
          do {
            uVar3 = AoWE_TUnitList_GetUnit_23EDC2EF(piVar4,iVar5);
            if (*(int *)(iVar2 + 0x14) < 0x3fffffff) {
              iVar6 = AoWE_TAbstractUnit_GetStrengthMax_23EDC2EF
                                (uVar3,CONCAT22((short)(~CONCAT22(extraout_var,0x100) >> 0x10),
                                                (ushort)~CONCAT22(extraout_var,0x100) &
                                                AoWE_dtAll_3609ABCE));
              *(int *)(iVar2 + 0x14) = *(int *)(iVar2 + 0x14) + iVar6;
            }
            iVar5 = iVar5 + -1;
          } while (iVar5 != -1);
        }
        iStack_18 = iStack_18 + 1;
        iStack_14 = iStack_14 + -1;
      } while (iStack_14 != 0);
    }
  }
  return;
}

