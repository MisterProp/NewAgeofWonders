// /AoWEPACK.dpl : 5575d870

void AoWE_TProductionControl_ResearchUnit_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  int *in_FS_OFFSET;
  int iStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_10;
  undefined1 local_9;
  undefined4 local_8;
  
                    /* 0x5d870  2335  AoWE.TProductionControl.ResearchUnit@23EDC2EF */
  puStack_20 = &stack0xfffffffc;
  local_10 = 0;
  local_9 = (undefined1)param_3;
  puStack_24 = &LAB_5575d94b;
  iStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_28;
  local_8 = param_2;
  cVar2 = (**(code **)(*param_1 + 0xf0))(param_1,param_2,param_3);
  if (cVar2 != '\0') {
    cVar2 = (**(code **)(*param_1 + 0x7c))();
    AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar2)
    ;
    cVar2 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar2 == '\0') {
      piVar3 = (int *)(**(code **)(*param_1 + 0x74))();
      piVar3[5] = 0;
      *(undefined1 *)(piVar3 + 7) = local_9;
      iVar4 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionItem_E3617239,1,0);
      piVar3[6] = iVar4;
      *(undefined4 *)(iVar4 + 8) = 2;
      *(undefined4 *)(iVar4 + 0xc) = local_8;
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      piVar5 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)cVar2);
      (**(code **)(*piVar5 + 0x10))(piVar5,piVar3);
      (**(code **)(*piVar3 + 0x2c))();
    }
  }
  puVar1 = puStack_24;
  *in_FS_OFFSET = (int)&local_10;
  puStack_24 = &LAB_5575d952;
  iStack_28 = 0x5575d94a;
  System__LStrClr_40929B27(&local_10,&local_10,puVar1);
  return;
}

