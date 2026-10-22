
uint Farm_TFarm_UpdateAITarget_23EDC2EF(int *param_1,byte *param_2,int *param_3)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  
                    /* 0xb2d40  7104  Farm.TFarm.UpdateAITarget@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x150))();
  if ((cVar1 != '\0') &&
     (uVar2 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1), (char)uVar2 == '\0')) {
    return uVar2;
  }
  uVar2 = (uint)(byte)(param_2[2] - 1);
  if (((param_2[2] == 0) && (uVar2 = (uint)*param_2, *param_2 != *(byte *)(param_1 + 0xc))) &&
     (uVar2 = (**(code **)(*param_1 + 0x150))(), (char)uVar2 == '\0')) {
    iVar3 = *param_3;
    if ((char)param_1[0xc] == '\0') {
      iVar3 = 0x32;
    }
    else {
      uVar2 = AoWE_ValidAttackRelation_C8BA104C(*param_2,param_2[1],(char)param_1[0xc],0);
      if ((char)uVar2 != '\0') {
        iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                           (int)(char)*param_2);
        uVar2 = (int)*(uint *)(iVar3 + 0xd0) >> 1;
        if ((int)uVar2 < 0) {
          uVar2 = uVar2 + ((*(uint *)(iVar3 + 0xd0) & 1) != 0);
        }
        iVar3 = 0x32 - uVar2;
        if (iVar3 < 0x19) {
          iVar3 = 0x19;
        }
        if (0x4b < iVar3) {
          iVar3 = 0x4b;
        }
      }
    }
    if (*param_3 < iVar3) {
      *param_3 = iVar3;
    }
  }
  return uVar2;
}

