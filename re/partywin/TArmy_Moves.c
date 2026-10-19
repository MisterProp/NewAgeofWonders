
char AoWE_TArmy_Moves_23EDC2EF(int *param_1,uint param_2)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  char local_18;
  
                    /* 0x8d95c  395  AoWE.TArmy.Moves@23EDC2EF */
  piVar2 = (int *)AoWE_TArmy_Transporter_23EDC2EF(param_1,param_2);
  if (piVar2 == (int *)0x0) {
    if ((char)param_2 == '\0') {
      local_18 = '\0';
    }
    else {
      local_18 = '2';
      iVar3 = (**(code **)(*param_1 + 0x54))();
      if (-1 < iVar3 + -1) {
        iVar4 = 0;
        do {
          if ((param_2 & 1) != 0) {
            piVar2 = *(int **)(*(int *)(param_1[2] + 4) + iVar4 * 4);
            cVar1 = (**(code **)(*piVar2 + 0xd4))();
            if (cVar1 < local_18) {
              local_18 = (**(code **)(*piVar2 + 0xd4))();
            }
          }
          param_2 = (param_2 & 0xff) >> 1;
          iVar4 = iVar4 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
  }
  else {
    local_18 = (**(code **)(*piVar2 + 0xd4))();
  }
  return local_18;
}

