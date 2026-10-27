

void City_TCity_CheckRebellion_23EDC2EF(int *param_1)



{

  undefined1 *puVar1;

  byte bVar2;

  char cVar3;

  char cVar4;

  int iVar5;

  int *piVar6;

  undefined4 uVar7;

  undefined4 uVar8;

  undefined4 uVar9;

  int iVar10;

  uint extraout_EDX;

  int iVar11;

  undefined4 *in_FS_OFFSET;

  undefined4 uVar12;

  undefined4 uStack_20;

  undefined1 *puStack_1c;

  undefined1 *puStack_18;

  undefined4 local_8;

  

                    /* 0xab534  6875  City.TCity.CheckRebellion@23EDC2EF */

  puStack_18 = &stack0xfffffffc;

  local_8 = 0;

  puStack_1c = &LAB_557ab6ca;

  uStack_20 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_20;

  puVar1 = &stack0xfffffffc;

  if ((char)param_1[0xc] != '\0') {

    bVar2 = City_TCity_GetCivilStatus_23EDC2EF(param_1);

    puVar1 = puStack_18;

    if (bVar2 < 2) {

      cVar3 = (**(code **)(*param_1 + 0x150))();

      puVar1 = puStack_18;

      if (cVar3 == '\0') {

        if (bVar2 == 0) {

          iVar11 = 0x32;

        }

        else {

          iVar11 = 10;

        }

        iVar5 = AoWE_TAoWHSMap_Random_23EDC2EF

                          (*(undefined4 *)PTR_AoWE_AoWHSMap_6B957A58_558e9494,100);

        puVar1 = puStack_18;

        if (iVar5 <= iVar11) {

          City_TCity_MoveArmiesOutOfCity_23EDC2EF(param_1,extraout_EDX & 0xffffff00);

          piVar6 = (int *)EventLog_TEventLog_Create_23EDC2EF

                                    (&City__TCityRebellionEventLog_0F2F3852,1,0);

          *(char *)(piVar6 + 4) = (char)param_1[0xc];

          (**(code **)(*param_1 + 0x148))(param_1,&local_8);

          System__LStrAsg_51F89FF7(piVar6 + 9,local_8);

          cVar3 = (**(code **)(*param_1 + 0x74))();

          piVar6[6] = (int)cVar3;

          cVar3 = (**(code **)(*param_1 + 0x78))();

          piVar6[7] = (int)cVar3;

          cVar3 = (**(code **)(*param_1 + 0x7c))();

          piVar6[8] = (int)cVar3;

          uVar7 = (**(code **)(*param_1 + 0x78))();

          uVar8 = (**(code **)(*param_1 + 0x7c))();

          uVar12 = 0;

          uVar9 = (**(code **)(*param_1 + 0x74))();

          EventLog_DistributeLocationEventLog_899ECAE1

                    (piVar6,(char)param_1[0xc],uVar9,uVar12,uVar8,uVar7);

          (**(code **)(*piVar6 + 0x2c))();

          (**(code **)(*param_1 + 0x1f0))(param_1,0);

          piVar6 = (int *)Engine_TEChangeNotifyNode_Create_23EDC2EF(&AoWE__TUnitList_56F39DF3,1,0);

          City_TCity_GenerateRebelUnits_23EDC2EF(param_1,piVar6);

          iVar11 = (**(code **)(*piVar6 + 0x54))();

          iVar11 = iVar11 + -1;

          if (-1 < iVar11) {

            do {

              cVar3 = (**(code **)(*param_1 + 0x7c))();

              iVar5 = (int)cVar3;

              uVar8 = 0;

              iVar10 = (uint)(uint3)(cVar3 >> 7) << 8;

              cVar3 = (**(code **)(*param_1 + 0x78))();

              cVar4 = (**(code **)(*param_1 + 0x74))();

              uVar7 = AoWE_TUnitList_GetUnit_23EDC2EF(piVar6,iVar11);

              AoWE_TAbstractUnit_Place_23EDC2EF(uVar7,(int)cVar4,(int)cVar3,iVar10,uVar8,iVar5);

              iVar11 = iVar11 + -1;

            } while (iVar11 != -1);

          }

          System_TObject_Free_23EDC2EF(piVar6);

          (**(code **)(*(int *)param_1[0x14] + 0x6c))();

          puVar1 = puStack_18;

        }

      }

    }

  }

  puStack_18 = puVar1;

  puVar1 = puStack_18;

  *in_FS_OFFSET = uStack_20;

  puStack_18 = &LAB_557ab6d1;

  puStack_1c = (undefined1 *)0x557ab6c9;

  System__LStrClr_40929B27(&local_8,uStack_20,puVar1);

  return;

}




