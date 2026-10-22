
void FUN_00443f88(undefined4 param_1,int *param_2)

{
  char cVar1;
  
  cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_2);
  if (cVar1 == '\0') {
    cVar1 = (**(code **)(*param_2 + 0x150))();
    if (cVar1 == '\0') {
      cVar1 = System__IsClass_51F89FF7(param_2,AoWE__TPlayerStructure_5B95B607_exref);
      if (cVar1 == '\0') {
        cVar1 = System__IsClass_51F89FF7(param_2,Sign__TSign_331ED5F9_exref);
        if (cVar1 == '\0') {
          cVar1 = System__IsClass_51F89FF7(param_2,Cave__TCave_2512A4D4_exref);
          if (cVar1 == '\0') {
            cVar1 = System__IsClass_51F89FF7(param_2,Teleport__TTeleport_39BEC537_exref);
            if (cVar1 == '\0') {
              cVar1 = System__IsClass_51F89FF7(param_2,ExploreS__TExplorationSite_304DA678_exref);
              if (cVar1 == '\0') {
                cVar1 = System__IsClass_51F89FF7(param_2,WizardsTower__TWizardsTower_C536AB7D_exref)
                ;
                if (cVar1 == '\0') {
                  cVar1 = System__IsClass_51F89FF7(param_2,Arena__TArena_2050A69A_exref);
                  if (cVar1 == '\0') {
                    FUN_00443f44(param_1,param_2);
                  }
                  else {
                    FUN_00442d08(param_1,param_2);
                  }
                }
                else {
                  FUN_00442c48(param_1,param_2);
                }
              }
              else {
                FUN_00442f08(param_1,param_2);
              }
            }
            else {
              FUN_00442b64(param_1,param_2);
            }
          }
          else {
            FUN_00442dc8(param_1,param_2);
          }
        }
        else {
          FUN_00442b20(param_1,param_2);
        }
      }
      else if (((char)param_2[0xc] == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) ||
              (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6c) != '\0')) {
        cVar1 = System__IsClass_51F89FF7(param_2,City__TCity_01CDE5D7_exref);
        if (cVar1 == '\0') {
          cVar1 = System__IsClass_51F89FF7(param_2,PrdPlace__TProductionPlace_9A13A4CB_exref);
          if (cVar1 == '\0') {
            cVar1 = System__IsClass_51F89FF7(param_2,Mine__TMine_BF169B80_exref);
            if (cVar1 == '\0') {
              cVar1 = System__IsClass_51F89FF7(param_2,PowerNode__TPowerNode_152A66F6_exref);
              if (cVar1 == '\0') {
                cVar1 = System__IsClass_51F89FF7(param_2,Farm__TFarm_413B1490_exref);
                if (cVar1 == '\0') {
                  cVar1 = System__IsClass_51F89FF7(param_2,Tower__TTower_D77EA595_exref);
                  if (cVar1 == '\0') {
                    cVar1 = System__IsClass_51F89FF7(param_2,Altar__TAltar_E5398657_exref);
                    if (cVar1 == '\0') {
                      FUN_00443f44(param_1,param_2);
                    }
                    else {
                      FUN_00443078(param_1,param_2);
                    }
                  }
                  else {
                    FUN_00442eb8(param_1,param_2);
                  }
                }
                else {
                  FUN_00442a20(param_1,param_2);
                }
              }
              else {
                FUN_004428b8(param_1,param_2);
              }
            }
            else {
              FUN_00442804(param_1,param_2);
            }
          }
          else {
            cVar1 = (**(code **)(*param_2 + 0x230))();
            if (cVar1 == '\b') {
              FUN_0044377c(param_1,param_2);
            }
            else {
              FUN_0044296c(param_1,param_2);
            }
          }
        }
        else if ((*(byte *)(param_2 + 0x11) & 1) == 0) {
          FUN_00443318(param_1,param_2);
        }
        else {
          FUN_00443218(param_1,param_2);
        }
      }
      else {
        cVar1 = System__IsClass_51F89FF7(param_2,City__TCity_01CDE5D7_exref);
        if ((cVar1 == '\0') || ((*(byte *)(param_2 + 0x11) & 1) == 0)) {
          FUN_00443dd4(param_1,param_2);
        }
        else {
          FUN_00443218(param_1,param_2);
        }
      }
    }
    else {
      FUN_00443c90(param_1,param_2);
    }
  }
  else {
    FUN_00443c00(param_1,param_2);
  }
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x6c) + 0x75) == '\0') {
    (**(code **)(**(int **)(DAT_0045b208 + 0x44) + 0x6c))(*(int **)(DAT_0045b208 + 0x44),1);
  }
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x6c) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x6c),1);
  return;
}

