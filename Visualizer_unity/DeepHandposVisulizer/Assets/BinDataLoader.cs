using UnityEngine;
using System.Collections;

public class BinDataLoader : MonoBehaviour {

    public GameObject Origin, Calc;
    public string FilePath;

	// Use this for initialization
	void Start () {
        Calc.GetComponent<HandController>().setColor(Color.blue);
        Origin.GetComponent<HandController>().setColor(Color.green);
    }
	
	// Update is called once per frame
	void Update () {
        //Read
    }
}
