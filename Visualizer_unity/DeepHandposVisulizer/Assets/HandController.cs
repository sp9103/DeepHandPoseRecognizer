using UnityEngine;
using System.Collections;

public class HandController : MonoBehaviour
{
    public GameObject Thumb, Index, Middle, Ring, Pinky;

    private Vector3[,] boneArray = new Vector3[5, 4];
    private float scaleFactor = 0.01f;
    private GameObject[,] Bone = new GameObject[5, 4];

    // Use this for initialization
    void Start()
    {
        //손가락 뼈 실린더 생성부
        for(int i = 0; i < 5; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                Bone[i, j] = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
                Bone[i, j].transform.parent = this.transform;
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        jointMove();
    }

    public void setJointPos(float[] pos)
    {
        for (int f = 0; f < 5; f++)
        {
            for (int b = 0; b < 4; b++)
            {
                boneArray[f, b] = new Vector3(pos[f * 4 * 3 + b * 3 + 0], pos[f * 4 * 3 + b * 3 + 1], -pos[f * 4 * 3 + b * 3 + 2]) * scaleFactor;
            }
        }
    }

    public void setBone(float[] prev, float[] next)
    {
        for (int f = 0; f < 5; f++)
        {
            for (int b = 0; b < 4; b++)
            {
                Vector3 objPos = new Vector3(prev[f * 4 * 3 + b * 3 + 0] + next[f * 4 * 3 + b * 3 + 0],
                   prev[f * 4 * 3 + b * 3 + 1] + next[f * 4 * 3 + b * 3 + 1], -(prev[f * 4 * 3 + b * 3 + 2] + next[f * 4 * 3 + b * 3 + 2])) * scaleFactor / 2.0f ;
                Vector3 direction = new Vector3(prev[f * 4 * 3 + b * 3 + 0] - next[f * 4 * 3 + b * 3 + 0],
                    prev[f * 4 * 3 + b * 3 + 1] - next[f * 4 * 3 + b * 3 + 1], -(prev[f * 4 * 3 + b * 3 + 2] - next[f * 4 * 3 + b * 3 + 2]));

                Bone[f, b].transform.position = objPos;
                Bone[f, b].transform.up = direction;
                //Bone[f, b].transform.localScale
            }
        }
    }

    void jointMove()
    {
        fingerMove(Thumb, boneArray, 0);
        fingerMove(Index, boneArray, 1);
        fingerMove(Middle, boneArray, 2);
        fingerMove(Ring, boneArray, 3);
        fingerMove(Pinky, boneArray, 4);
    }

    void fingerMove(GameObject finger, Vector3[,] pos, int idx)
    {
        Transform[] child = finger.transform.GetComponentsInChildren<Transform>(true);
        foreach (Transform t in child)
        {
            if (t.gameObject.name == "1")
            {
                t.gameObject.transform.position = pos[idx, 0];
            }
            if (t.gameObject.name == "2")
            {
                t.gameObject.transform.position = pos[idx, 1];
            }
            if (t.gameObject.name == "3")
            {
                t.gameObject.transform.position = pos[idx, 2];
            }
            if (t.gameObject.name == "4")
            {
                t.gameObject.transform.position = pos[idx, 3];
            }
        }
    }
}
