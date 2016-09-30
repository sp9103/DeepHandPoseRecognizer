using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;

public class LeepController : MonoBehaviour {

    [DllImport("LeepDll")]
    private static extern void TEST();
    [DllImport("LeepDll")]
    private static extern void Init();
    [DllImport("LeepDll")]
    private static extern void CalcProbabilty(float[] prob);

    public GameObject HandModel;
    public Slider[] probSlider = new Slider[10];

    //GEST_COUNT
    float[] prob = new float[10];
	// Use this for initialization
	void Start () {
        Init();
	}
	
	// Update is called once per frame
	void Update () {

        CalcProbabilty(prob);
        for(int i = 0; i < 10; i++)
        {
            probSlider[i].value = prob[i];
        }
	}
}
