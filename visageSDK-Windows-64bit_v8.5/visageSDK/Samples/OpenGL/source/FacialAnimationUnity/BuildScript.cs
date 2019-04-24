using UnityEngine;
using UnityEditor;

public class CommandBuild
{
	
    public static void SetAndroidBuild(string outPath)
    {
		string[] scenes = { "Assets/Visage Tracker/Jones/Jones.unity"};
		PlayerSettings.companyName = "VisageTechnologies";
		PlayerSettings.productName = "FacialAnimationUnityDemo";
        PlayerSettings.SetApplicationIdentifier(BuildTargetGroup.Android, "com.visagetechnologies.facialanimationunitydemo");

        PlayerSettings.bundleVersion = "1.0";
        PlayerSettings.Android.bundleVersionCode = 1;
        PlayerSettings.SetMobileMTRendering(BuildTargetGroup.Android, false);
		EditorUserBuildSettings.androidBuildSystem = AndroidBuildSystem.Internal;
        BuildPipeline.BuildPlayer(scenes, "../../Android/" + outPath, BuildTarget.Android, BuildOptions.None);
    }
	
	public static void SetWindowsBuild64(string outPath)
	{
		string[] scenes = { "Assets/Visage Tracker/Jones/Jones.unity"};
		
		BuildPipeline.BuildPlayer(scenes, "../../../packages/Unity/x86_64/" + outPath, BuildTarget.StandaloneWindows64, BuildOptions.None);
	}
	
	public static void SetWindowsBuild32(string outPath)
	{
		string[] scenes = { "Assets/Visage Tracker/Jones/Jones.unity"};
		
		BuildPipeline.BuildPlayer(scenes, "../../../packages/Unity/x86_32/" + outPath, BuildTarget.StandaloneWindows, BuildOptions.None);
	}

    public static void BuildAndroid()
    {
        
        SetAndroidBuild("FacialAnimationUnityDemo-release.apk");
    }
	
	public static void BuildWindows64()
    {
        
        SetWindowsBuild64("FacialAnimationUnityDemo.exe");
    }
	
	public static void BuildWindows32()
    {
        
        SetWindowsBuild32("FacialAnimationUnityDemo.exe");
    }
}