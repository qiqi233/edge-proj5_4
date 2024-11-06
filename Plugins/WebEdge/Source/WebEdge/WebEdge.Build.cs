// Copyright CengJia.  Year : 2023

using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class WebEdge : ModuleRules
{
	private string ThirdPartyPath
	{
		get { return Path.Combine(ModuleDirectory, "../ThirdParty/"); }
	}
	private string ProjectPath
	{
		get { return Path.Combine(PluginDirectory, "../../"); }
	}
	
	public WebEdge(ReadOnlyTargetRules Target) : base(Target)
    {
	    if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			
			string srcDll_lib = Path.Combine(ThirdPartyPath, "Edge/dll_lib/");
			string dstDll_lib = Path.Combine(ProjectPath, "Binaries/Win64/");

			bEnableUndefinedIdentifierWarnings = false;
			PublicIncludePaths.Add(  Path.Combine(ThirdPartyPath, "Edge/include") );
			if (Target.Configuration == UnrealTargetConfiguration.Debug)
			{
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Debug/WebView2Loader.dll.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Debug/WebView2LoaderStatic.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Debug/EdgeUE.lib"));
				srcDll_lib += "Debug/";
			}
			else
			{
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Release/WebView2Loader.dll.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Release/WebView2LoaderStatic.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Edge/dll_lib/Release/EdgeUE.lib"));
				srcDll_lib += "Release/";
			}
			
			
			
			if(!Directory.Exists( Path.Combine(ProjectPath, "Binaries/Win64/")))
				Directory.CreateDirectory(Path.Combine(ProjectPath, "Binaries/Win64/"));
			
			PublicDelayLoadDLLs.Add("WebView2Loader.dll");
			PublicDelayLoadDLLs.Add("EdgeUE.dll");
			
			RuntimeDependencies.Add(srcDll_lib + "WebView2Loader.dll");
			RuntimeDependencies.Add(srcDll_lib +"EdgeUE.dll");
			var moduleBinaryFiles = Directory.GetFiles(srcDll_lib, "*", SearchOption.AllDirectories);
			foreach (var moduleBinaryFile in moduleBinaryFiles)
			{
				string DependencyName = Path.GetFileName(moduleBinaryFile);
				string des_file = Path.Combine(dstDll_lib, DependencyName);
				
				if (!File.Exists(des_file))
				{
					File.Copy(moduleBinaryFile,des_file);
				} 
			}
			
		}
	    
	    
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Slate",
				"SlateCore",
				"UMG",
				"Engine",
				"InputCore",
				"RHI",
				"Json"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
