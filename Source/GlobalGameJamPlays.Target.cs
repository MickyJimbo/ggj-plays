// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GlobalGameJamPlaysTarget : TargetRules
{
	public GlobalGameJamPlaysTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "GlobalGameJamPlays" } );
	}
}
