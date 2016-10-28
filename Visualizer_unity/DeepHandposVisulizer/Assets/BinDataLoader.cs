using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;

public class BinDataLoader : MonoBehaviour {
    [DllImport("LeepDll")]
    private static extern void getBinData(byte[] filePath, float[] orinext, float[] oriprev, float[] netnext, float[] netprev, int waitterm);

    public GameObject Origin, Calc;
    public string FilePath;
    public int waitTerm;

    private float[] oriArray = new float[5 * 4 * 3];
    private float[] oriPrev = new float[5 * 4 * 3];
    private float[] netArray = new float[5 * 4 * 3];
    private float[] netPrev = new float[5 * 4 * 3];

    // Use this for initialization
    void Start () {
        Calc.GetComponent<HandController>().setColor(Color.blue);
        Origin.GetComponent<HandController>().setColor(Color.green);
    }
	
	// Update is called once per frame
	void Update () {
        //Read
        byte[] file_char = System.Text.Encoding.UTF8.GetBytes(FilePath);

        Debug.Log(waitTerm);
        getBinData(file_char, oriArray, oriPrev, netArray, netPrev, waitTerm);

        //Move
        Calc.GetComponent<HandController>().setJointPos(netArray);
        Calc.GetComponent<HandController>().setBone(netPrev, netArray);
        Origin.GetComponent<HandController>().setJointPos(oriArray);
        Origin.GetComponent<HandController>().setBone(oriPrev, oriArray);
    }
}
