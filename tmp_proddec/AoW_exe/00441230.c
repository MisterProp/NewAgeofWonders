// /AoW.exe : 00441230

void LoopQueueBoxChange(int param_1)

{
  AoWE_TProductionControl_SetLoopQueueTo_23EDC2EF
            (*(undefined4 *)(*(int *)(param_1 + 0x1e8) + 0x44),
             CONCAT31((int3)((uint)*(int *)(param_1 + 0x158) >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x158) + 0xb0)));
  return;
}

