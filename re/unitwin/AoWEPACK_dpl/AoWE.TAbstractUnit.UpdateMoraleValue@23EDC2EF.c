// /AoWEPACK.dpl : AoWE.TAbstractUnit.UpdateMoraleValue@23EDC2EF

undefined1 AoWE_TAbstractUnit_UpdateMoraleValue_23EDC2EF(int *param_1)

{
  int *piVar1;
  char cVar2;
  byte bVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  
                    /* 0x7f058  1091  AoWE.TAbstractUnit.UpdateMoraleValue@23EDC2EF */
  iVar6 = (int)(char)param_1[9];
  uVar4 = (**(code **)(*param_1 + 0x170))();
  iVar5 = (**(code **)(*param_1 + 0x17c))(param_1,iVar6,uVar4);
  if ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 2) == 0) {
    cVar2 = (**(code **)(*param_1 + 0x114))();
    if (cVar2 != '\x02') {
      cVar2 = System__IsClass_51F89FF7(param_1,&AoWE__TLeader_DF642EA6);
      if (cVar2 == '\0') {
        cVar2 = System__IsClass_51F89FF7(param_1[1],&AoWE__TArmy_228DACC0);
        if (cVar2 != '\0') {
          cVar2 = (**(code **)(*param_1 + 0x9c))();
          if ((cVar2 != '\0') && (0 < iVar6)) {
            cVar2 = (**(code **)(*param_1 + 0x148))(param_1,0x9b);
            if (cVar2 == '\0') {
              iVar6 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar6);
              iVar5 = iVar5 + *(char *)(iVar6 + 0x50);
            }
          }
          iVar6 = (**(code **)(*(int *)param_1[1] + 0x54))();
          iVar6 = iVar6 + -1;
          if (-1 < iVar6) {
            do {
              piVar1 = *(int **)(*(int *)(*(int *)(param_1[1] + 8) + 4) + iVar6 * 4);
              cVar2 = (**(code **)(*piVar1 + 0x148))(piVar1,0x25);
              if ((cVar2 == '\0') || (param_1 == piVar1)) {
                bVar3 = AoWE_TAbstractUnit_UnitRelation_23EDC2EF(param_1,piVar1);
                if (bVar3 < 2) {
                  iVar5 = iVar5 + -10;
                }
              }
              else {
                iVar5 = iVar5 + 0x1e;
              }
              iVar6 = iVar6 + -1;
            } while (iVar6 != -1);
          }
        }
      }
    }
  }
  if (iVar5 < 0x7e) {
    if (iVar5 < -0x19) {
      *(undefined1 *)((int)param_1 + 0x26) = 0xe7;
    }
    else {
      *(char *)((int)param_1 + 0x26) = (char)iVar5;
    }
  }
  else {
    *(undefined1 *)((int)param_1 + 0x26) = 0x7d;
  }
  return *(undefined1 *)((int)param_1 + 0x26);
}

