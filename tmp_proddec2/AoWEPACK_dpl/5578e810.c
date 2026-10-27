// /AoWEPACK.dpl : 5578e810

int AoWE_TArmy_AddChild_23EDC2EF(int param_1,int *param_2)

{
  char cVar1;
  int iVar2;
  
                    /* 0x8e810  367  AoWE.TArmy.AddChild@23EDC2EF */
  if (((*(byte *)(param_1 + 0x10) & 1) != 0) &&
     (cVar1 = (**(code **)(*param_2 + 0x180))(), cVar1 == '\0')) {
    return -1;
  }
  iVar2 = AoWE_TUnitList_AddChild_23EDC2EF(param_1,param_2);
  if (iVar2 != -1) {
    if (((*(byte *)(param_1 + 0x10) & 1) != 0) && ((*(byte *)((int)param_2 + 0x25) & 2) == 0)) {
      (**(code **)(*param_2 + 0x184))();
    }
    (**(code **)(*param_2 + 0x16c))(param_2,*(undefined1 *)(param_1 + 0x12));
  }
  return iVar2;
}

