using UnityEngine;
using UnityEditor;

public class CommandBuild
{
	
    public static void SetAndroidBuild(string outPath)
    {
		string[] scenes = { "Assets/Main.unity"};
		PlayerSettings.companyName = "VisageTechnologies";
		PlayerSettings.productName = "VisageTrackerUnityDemo";
        PlayerSettings.SetApplicationIdentifier(BuildTargetGroup.Android, "com.visagetechnologies.visagetrackerunitydemo");

        PlayerSettings.bundleVersion = "1.0";
        PlayerSettings.Android.bundleVersionCode = 1;
        PlayerSettings.SetMobileMTRendering(BuildTargetGroup.Android, false);
		EditorUserBuildSettings.androidBuildSystem = AndroidBuildSystem.Internal;
        BuildPipeline.BuildPlayer(scenes, "../../Android/" + outPath, BuildTarget.Android, BuildOptions.None);
    }
	
	public static void SetWindowsBuild64(string outPath)
	{
		string[] scenes = { "Assets/Main.unity"};
		
		BuildPipeline.BuildPlayer(scenes, "../../../packages/Unity/x86_64/" + outPath, BuildTarget.StandaloneWindows64, BuildOptions.None);
	}
	
	public static void SetWindowsBuild32(string outPath)
	{
		string[] scenes = { "Assets/Main.unity"};
		
		BuildPipeline.BuildPlayer(scenes, "../../../packages/Unity/x86_32/" + outPath, BuildTarget.StandaloneWindows, BuildOptions.None);
	}

    public static void BuildAndroid()
    {
        
        SetAndroidBuild("VisageTrackerUnityDemo-release.apk");
    }
	
	public static void BuildWindows64()
    {
        
        SetWindowsBuild64("VisageTrackerUnityDemo.exe");
    }
	
	public static void BuildWindows32()
    {
        
        SetWindowsBuild32("VisageTrackerUnityDemo.exe");
    }
}