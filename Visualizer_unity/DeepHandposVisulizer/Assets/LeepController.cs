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
    private static extern void LeapUpdataFrame();
    [DllImport("LeepDll")]
    private static extern void LeapShowImage(byte[] left, byte[] right, int width_height);
    [DllImport("LeepDll")]
    private static extern void CalcProbabilty(float[] prob);
    [DllImport("LeepDll")]
    private static extern int getPalmData(float[] pos, float[] norm, float[] dir);

    public GameObject HandModel;
    public Slider[] probSlider = new Slider[10];
    public Image[] LeapImage = new Image[2];

    //GEST_COUNT
    private float[] prob = new float[10];
    private float[] palm_pos = new float[3];
    private float[] palm_norm = new float[3];
    private float[] palm_dir = new float[3];
    private byte[] left;
    private byte[] right;
    private Texture2D camTexture;
    private Color[] imagePixel;
    private float scale_factor = 0.01f;

    // Use this for initialization
    void Start () {
        Init();

        int width_height = (int)LeapImage[0].rectTransform.rect.width;
        left = new byte[width_height * width_height];
        right = new byte[width_height * width_height];
        camTexture = new Texture2D(width_height, width_height, TextureFormat.RGBA32, false);
        imagePixel = new Color[width_height * width_height];
    }
	
	// Update is called once per frame
	void Update () {
        LeapUpdataFrame();
        setCamFrame();
        setPalm();
        visProb();
    }

    void visProb()
    {
        CalcProbabilty(prob);
        for (int i = 0; i < 10; i++)
        {
            probSlider[i].value = prob[i];
        }
    }

    void setPalm()
    {
        int check = getPalmData(palm_pos, palm_norm, palm_dir);

        if (check == -1)
            HandModel.SetActive(false);
        else
        {
            HandModel.SetActive(true);
            Transform[] child = HandModel.transform.GetComponentsInChildren<Transform>(true);
            foreach(Transform t in child)
            {
                if(t.gameObject.name == "R_Palm")
                {
                    Vector3 pos = new Vector3(palm_pos[0] * scale_factor, palm_pos[1] * scale_factor, palm_pos[2] * scale_factor);
                    Vector3 norm = new Vector3(palm_norm[0], palm_norm[1], palm_norm[2]);
                    Vector3 dir = new Vector3(palm_dir[0], palm_dir[1], palm_dir[2]);
                    t.gameObject.transform.position = pos;
                    //t.gameObject.transform.rotation.
                }
            }
        }
    }

    void setCamFrame()
    {
        int width_height = (int)LeapImage[0].rectTransform.rect.width;
        LeapShowImage(left, right, width_height);
        //byteToImage(left, LeapImage[0]);
        //byteToImage(right, LeapImage[1]);
    }

    void byteToImage(byte[] src, Image dst)
    {
        int width_height = (int)LeapImage[0].rectTransform.rect.width;

        for (int i = 0; i < width_height * width_height; i++)
        {
            imagePixel[i].a = 1;
            imagePixel[i].r = 1;
            imagePixel[i].g = 1;
            imagePixel[i].b = 1;
        }

        camTexture.SetPixels(imagePixel, width_height * width_height);
        Sprite tempSprite = Sprite.Create(camTexture, new Rect(0, 0, camTexture.width, camTexture.height), Vector2.zero, 0);
        dst.sprite = tempSprite;
    }
}
