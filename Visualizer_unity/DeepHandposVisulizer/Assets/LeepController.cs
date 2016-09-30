using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;

public class LeepController : MonoBehaviour {

    [DllImport("LeepDll")]
    private static extern void TEST();

    public GameObject HandModel;
	// Use this for initialization
	void Start () {
        //TEST();
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
