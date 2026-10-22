
void AoWE_TPlayerControl_Setup_23EDC2EF(undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
                    /* 0x54b94  2553  AoWE.TPlayerControl.Setup@23EDC2EF */
  if ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x13a) == '\x01') &&
     ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 2) == 0)) {
    if (*(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x240) + 0x1c) == 0) {
      SysUtils_Exception_Create_23EDC2EF
                (SysUtils__Exception_9880D0BF_exref,
                 CONCAT31((int3)((uint)*(int *)(AoWE_AoWHSMap_6B957A58 + 0x240) >> 8),1),
                 "Please setup DirectPlayPCC");
      param_1 = System__RaiseExcept_51F89FF7();
    }
    iVar3 = *(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x23c) + 4);
    *(undefined4 *)(iVar3 + 0x3c) = param_1;
    *(code **)(iVar3 + 0x38) = AoWE_TPlayerControl_GetSyncValue_23EDC2EF;
    *(undefined4 *)(iVar3 + 0x34) = param_1;
    *(code **)(iVar3 + 0x30) = AoWE_TPlayerControl_OutOfSync_23EDC2EF;
  }
  else if ((((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0x3c) & 2) != 0) ||
           ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x13a) == '\0' ||
            (*(char *)(AoWE_AoWHSMap_6B957A58 + 0x13a) == '\x02')))) &&
          (*(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x240) + 0x1c) == 0)) {
    uVar1 = NetworkE_TLocalPCC_Create_23EDC2EF
                      (NetworkE__TLocalPCC_9459F287_exref,
                       CONCAT31((int3)((uint)AoWE_AoWHSMap_6B957A58 >> 8),1));
    uVar2 = (**(code **)(**(int **)(AoWE_AoWHSMap_6B957A58 + 0x140) + 0x54))();
    NetworkE_TLocalPCC_Setup_23EDC2EF(uVar1,uVar2);
    iVar3 = (**(code **)(**(int **)(AoWE_AoWHSMap_6B957A58 + 0x140) + 0x54))();
    if (-1 < iVar3 + -1) {
      iVar5 = 0;
      do {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar5);
        if (*(char *)(iVar4 + 0xa7) == '\0') {
          NetworkE_TLocalPCC_SetConnectionMode_23EDC2EF(uVar1,iVar5,0);
        }
        else {
          NetworkE_TLocalPCC_SetConnectionMode_23EDC2EF(uVar1,iVar5,2);
        }
        iVar5 = iVar5 + 1;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    (**(code **)(**(int **)(AoWE_AoWHSMap_6B957A58 + 0x240) + 0x4c))
              (*(int **)(AoWE_AoWHSMap_6B957A58 + 0x240),uVar1);
    NetworkE_TPlayerCommunicationControl_Release_23EDC2EF(uVar1);
    AoWE_TPlayerList_Update_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140));
  }
  return;
}

