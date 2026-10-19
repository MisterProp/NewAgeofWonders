
undefined1 AoWE_TArmy_MoveTypes_23EDC2EF(int *param_1,uint param_2,undefined4 param_3)

{
  undefined1 uVar1;
  byte bVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  byte bVar6;
  uint uVar7;
  
                    /* 0x8d9e8  394  AoWE.TArmy.MoveTypes@23EDC2EF */
  piVar3 = (int *)AoWE_TArmy_Transporter_23EDC2EF(param_1,param_2,param_3,param_3);
  if (piVar3 == (int *)0x0) {
    uVar7 = 0;
    iVar4 = (**(code **)(*param_1 + 0x54))();
    uVar1 = (undefined1)uVar7;
    if (-1 < iVar4 + -1) {
      iVar5 = 0;
      do {
        bVar6 = (byte)uVar7;
        if ((param_2 & 1) != 0) {
          bVar2 = (**(code **)(**(int **)(*(int *)(param_1[2] + 4) + iVar5 * 4) + 0xe8))();
          uVar7 = (uint)(bVar2 | bVar6);
        }
        uVar1 = (undefined1)uVar7;
        param_2 = (param_2 & 0xff) >> 1;
        iVar5 = iVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
  }
  else {
    uVar1 = (**(code **)(*piVar3 + 0xe8))();
  }
  return uVar1;
}

