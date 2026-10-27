
void Gems_TGems_MsgProc_23EDC2EF(undefined4 param_1,int *param_2)

{
                    /* 0xc50c4  7739  Gems.TGems.MsgProc@23EDC2EF */
  Engine_TEObject_MsgProc_23EDC2EF(param_1,param_2);
  if ((*param_2 == 0x20020001) &&
     (*(int *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x174) == 1)) {
    Gems_TGems_GenerateGemsAmount_23EDC2EF(param_1);
  }
  return;
}

