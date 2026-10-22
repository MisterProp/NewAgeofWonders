
void Crops_TCrop_EndTurn_23EDC2EF(int *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  
                    /* 0xa5cfc  6761  Crops.TCrop.EndTurn@23EDC2EF */
  iVar2 = AoWE_TAoWMapField_GetArmyHS_23EDC2EF(param_1[1]);
  if ((((iVar2 != 0) && ((char)param_2 == *(char *)(*(int *)(iVar2 + 0x1c) + 0x12))) &&
      ('\0' < *(char *)(param_1[7] + 0x30))) && ((char)param_2 != *(char *)(param_1[7] + 0x30))) {
    cVar1 = AoWE_TPlayerList_GetDiplomaticRelation_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       *(undefined1 *)(param_1[7] + 0x30),param_2);
    if (cVar1 == '\x01') {
      (**(code **)(*(int *)param_1[7] + 0x240))((int *)param_1[7],param_2);
      (**(code **)(*param_1 + -4))(param_1,1);
    }
  }
  return;
}

