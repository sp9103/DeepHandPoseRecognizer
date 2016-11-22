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
    //[DllImport("LeepDll")]
    //private static extern void LeapUpdataFrame();
    //[DllImport("LeepDll")]
    //private static extern void LeapShowImage(byte[] left, byte[] right, int width_height);
    //[DllImport("LeepDll")]
    //private static extern void CalcProbabilty(float[] prob);
    //[DllImport("LeepDll")]
    //private static extern int getPalmData(float[] pos, float[] norm, float[] dir);
    //[DllImport("LeepDll")]
    //private static extern void getFingerData(float[] next, float[] prev);
    [DllImport("LeepDll")]
    private static extern int recvHandData(float[] next, float[] prev, float[] prob);

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

    public Camera Cam;
    public GameObject Base;
    private GUIStyle gStyle;

    // Use this for initialization
    void Start () {
        Init();

        int width_height = (int)LeapImage[0].rectTransform.rect.width;
        left = new byte[width_height * width_height];
        right = new byte[width_height * width_height];
        camTexture = new Texture2D(width_height, width_height, TextureFormat.RGBA32, false);
        imagePixel = new Color[width_height * width_height];

        //GUI Text style Initialize
        gStyle = new GUIStyle();
        gStyle.fontSize = 40;
    }
	
	// Update is called once per frame
	void Update () {
        int checkHand = recvHandData(fingerArray, fingerPrev, prob);
        setHand(checkHand, fingerArray, fingerPrev);
        if (checkHand != 1)
            visProb(0, prob);
        else
            visProb(1, prob);
    }

    void visProb(int check, float[] prob)
    {
        if (check == 1)
        {
            //CalcProbabilty(prob);
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

    int setHand(int check, float[] fingerArray, float[] figerPrev)
    {
        if (check != 1)
            HandModel.SetActive(false);
        else
        {
            HandModel.SetActive(true);

            //Hand model class로 넣어줌
            //getFingerData(fingerArray, fingerPrev);
            HandModel.GetComponent<HandController>().setJointPos(fingerArray);
            HandModel.GetComponent<HandController>().setBone(fingerPrev, fingerArray);
        }

        return check;
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

    void OnGUI()
    {
        Vector3 t = Cam.WorldToScreenPoint(Base.transform.position);

        int target = maxOut(prob);

        if (target == 0)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width/2, UnityEngine.Screen.height - 400, 400, 400), "ㄱ", gStyle);
        }
        else if(target == 1)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width/2, UnityEngine.Screen.height - 400, 400, 400), "ㄴ", gStyle);
        }
        else if (target == 2)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width/2, UnityEngine.Screen.height - 400, 400, 400), "ㄷ", gStyle);
        }
        else if (target == 3)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㄹ", gStyle);
        }
        else if (target == 4)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅁ", gStyle);
        }
        else if (target == 5)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅂ", gStyle);
        }
        else if (target == 6)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅅ", gStyle);
        }
        else if (target == 7)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅇ", gStyle);
        }
        else if (target == 8)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅈ", gStyle);
        }
        else if (target == 9)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅊ", gStyle);
        }
        else if (target == 10)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅋ", gStyle);
        }
        else if (target == 11)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅌ", gStyle);
        }
        else if (target == 12)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅍ", gStyle);
        }
        else if (target == 13)
        {
            GUI.Label(new Rect(UnityEngine.Screen.width / 2, UnityEngine.Screen.height - 400, 400, 400), "ㅎ", gStyle);
        }
    }

    int maxOut(float[] arr)
    {
        float max = 0.0f;
        int maxIdx = -1;
        for(int i = 0; i < 14; i++)
        {
            if(max < arr[i])
            {
                max = arr[i];
                maxIdx = i;
            }
        }
        return maxIdx;
    }
}
