// Fill out your copyright notice in the Description page of Project Settings.

#include "StructureDefines.h"
#include "Database/EnumDefines.h"



FRoomInfo::FRoomInfo()
{
	RoomNo = 0;
	Port = 0;
	PlayerList.Empty();
	BattleState = EBattleState::Createing;
}
