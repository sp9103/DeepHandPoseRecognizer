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
    [DllImport("LeepDll")]
    private static extern void getFingerData(float[] next, float[] prev);

    public GameObject HandModel;
    public Slider[] probSlider = new Slider[14];
    public Image[] LeapImage = new Image[2];

    //GEST_COUNT
    private float[] prob = new float[14];
    private float[] palm_pos = new float[3];
    private float[] palm_norm = new float[3];
    private float[] palm_dir = new float[3];
    private float[] fingerArray = new float[5 * 4 * 3];
    private float[] fingerPrev = new float[5 * 4 * 3];
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
        int checkHand = setHand();
        if (checkHand == -1)
            visProb(0);
        else
            visProb(1);
    }

    void visProb(int check)
    {
        if (check == 1)
        {
            CalcProbabilty(prob);
            for (int i = 0; i < 14; i++)
            {
                probSlider[i].value = prob[i];
            }
        }
        else
        {
            for (int i = 0; i < 14; i++)
            {
                probSlider[i].value = 0.0f;
            }
        }
    }

    void setFingerPos()
    {

    }

    int setHand()
    {
        int check = getPalmData(palm_pos, palm_norm, palm_dir);

        if (check == -1)
            HandModel.SetActive(false);
        else
        {
            HandModel.SetActive(true);

            //Hand model class로 넣어줌
            getFingerData(fingerArray, fingerPrev);
            HandModel.GetComponent<HandController>().setJointPos(fingerArray);
            HandModel.GetComponent<HandController>().setBone(fingerPrev, fingerArray);
        }

        return check;
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
