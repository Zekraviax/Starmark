// Copyright Epic Games, Inc. All Rights Reserved.



using UnrealBuildTool;

using System.Collections.Generic;



public class StarmarkServerTarget : TargetRules //Change this line according to the name of your project

{

    public StarmarkServerTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project

    {

        Type = TargetType.Server;

        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.Add("Starmark"); //Change this line according to the name of your project

    }

}