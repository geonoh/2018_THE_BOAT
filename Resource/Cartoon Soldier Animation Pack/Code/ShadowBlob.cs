using UnityEngine;
using System.Collections;

public class ShadowBlob : MonoBehaviour {
	
	public GameObject shadow;
	public GameObject player;
	public float shadowHeight = .05f;

	void Start()
	{
		shadow = GameObject.FindGameObjectWithTag("Shadow");
		player = GameObject.FindGameObjectWithTag("Player");
	}

	void Update()
	{
		Vector3 rayoffset = new Vector3(0,1f,0);
		Vector3 shadowOffset = new Vector3(0,shadowHeight,0);
		float distanceToGround;

		RaycastHit hit;
		
		if (Physics.Raycast(player.transform.position + rayoffset, -Vector3.up, out hit, 100f))
		{
			distanceToGround = hit.distance;
			Vector3 thePosition = hit.point + shadowOffset;
			shadow.transform.position = thePosition;
			float x = 1 - Mathf.Clamp(distanceToGround,0,8)/12f;
			float scale = Mathf.Lerp(.25f, .75f,x);
			shadow.transform.localScale = new Vector3(scale, scale, scale);
			shadow.transform.rotation = Quaternion.FromToRotation (Vector3.forward, hit.normal);
		}
		else
		{
			shadow.transform.position = player.transform.position + shadowOffset;
			shadow.transform.localScale = new Vector3(.75f, .75f, .75f);
		}
	}
}
