using UnityEngine;
using System.Collections;

public class BinDataLoader : MonoBehaviour {

    public GameObject HandModel;

    private GameObject Origin, Calc;

	// Use this for initialization
	void Start () {

        Origin = (GameObject)Instantiate(HandModel);
        Calc = (GameObject)Instantiate(HandModel);

        Calc.GetComponent<HandController>().setColor(Color.green);
        Origin.GetComponent<HandController>().setColor(Color.green);
    }
	
	// Update is called once per frame
	void Update () {
	
	}
}
